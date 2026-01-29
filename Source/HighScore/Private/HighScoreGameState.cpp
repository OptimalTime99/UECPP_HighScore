// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "HighScoreGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Item/SpawnVolume.h"
#include "Item/CoinItem.h"
#include "HighScoreGameInstance.h"
#include "Character/HighScorePlayerController.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Character/HighScoreCharacter.h"

AHighScoreGameState::AHighScoreGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    LevelDuration = 30.0f; // 한 레벨당 30초
    CurrentLevelIndex = 0;
    MaxLevels = 3;
}

void AHighScoreGameState::BeginPlay()
{
    Super::BeginPlay();

    // 게임 시작 시 첫 레벨부터 진행
    StartLevel();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &AHighScoreGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 AHighScoreGameState::GetScore() const
{
    return Score;
}

void AHighScoreGameState::AddScore(int32 Amount)
{
    UHighScoreGameInstance* MyGameInstance = Cast<UHighScoreGameInstance>(GetGameInstance());

    if (MyGameInstance)
    {
        MyGameInstance->AddToScore(Amount);
    }
}

void AHighScoreGameState::StartLevel()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (AHighScorePlayerController* HighScorePlayerController = Cast<AHighScorePlayerController>(PlayerController))
        {
            HighScorePlayerController->ShowGameHUD();
        }
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UHighScoreGameInstance* HighScoreGameInstance = Cast<UHighScoreGameInstance>(GameInstance);
        if (HighScoreGameInstance)
        {
            CurrentLevelIndex = HighScoreGameInstance->CurrentLevelIndex;
        }
    }

    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int32 ItemToSpawn = 40;

    if (FoundVolumes.Num() > 0)
    {
        ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
        if (SpawnVolume)
        {
            for (int32 i = 0; i < ItemToSpawn; i++)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        LevelTimerHandle,
        this,
        &AHighScoreGameState::OnLevelTimeUp,
        LevelDuration,
        false
    );
}

void AHighScoreGameState::OnLevelTimeUp()
{
    // 시간이 다 되면 레벨을 종료
    EndLevel();
}

void AHighScoreGameState::OnCoinCollected()
{
    CollectedCoinCount++;

    // 현재 레벨에서 스폰된 코인을 전부 주웠다면 즉시 레벨 종료
    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
    {
        EndLevel();
    }
}

void AHighScoreGameState::EndLevel()
{
    // 타이머 해제
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UHighScoreGameInstance* HighScoreGameInstance = Cast<UHighScoreGameInstance>(GameInstance);
        if (HighScoreGameInstance)
        {
            AddScore(Score);
            CurrentLevelIndex++;
            HighScoreGameInstance->CurrentLevelIndex = CurrentLevelIndex;

            if (CurrentLevelIndex >= MaxLevels)
            {
                OnGameOver();
                return;
            }

            if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
            {
                UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
            }
            else
            {
                OnGameOver();
            }
        }
    }
}

void AHighScoreGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        AHighScorePlayerController* HighScorePlayerController = Cast<AHighScorePlayerController>(PlayerController);
        {
            if (UUserWidget* HUDWidget = HighScorePlayerController->GetHUDWidget())
            {
                AHighScoreCharacter* MyCharacter = Cast<AHighScoreCharacter>(PlayerController->GetPawn());

                if (MyCharacter) // 캐릭터가 살아있을 때만 갱신
                {
                    if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("HP"))))
                    {
                        HPText->SetText(FText::FromString(FString::Printf(TEXT("%d"), (int32)MyCharacter->GetHealth())));
                    }

                    if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("PB_HP"))))
                    {
                        float MaxHealth = 100.0f;

                        if (MaxHealth > 0.0f)
                        {
                            float HPRatio = MyCharacter->GetHealth() / MaxHealth;
                            HPBar->SetPercent(HPRatio);
                        }
                    }
                }

                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
                    TimeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingTime)));
                }

                if (UProgressBar* TimeProgressBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("PB_Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);

                    // [중요] 0.0(0%) ~ 1.0(100%) 사이의 값으로 만들어야 합니다.
                    // LevelDuration이 0이면 나누기 에러가 나므로 안전장치 추가
                    if (LevelDuration > 0.f)
                    {
                        float Percent = RemainingTime / LevelDuration;
                        TimeProgressBar->SetPercent(Percent); // 게이지 설정 함수
                    }
                }

                if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
                {
                    if (UGameInstance* GameInstance = GetGameInstance())
                    {
                        UHighScoreGameInstance* HighScoreGameInstance = Cast<UHighScoreGameInstance>(GameInstance);
                        if (HighScoreGameInstance)
                        {
                            ScoreText->SetText(FText::FromString(FString::Printf(TEXT("SCORE: %d"), HighScoreGameInstance->TotalScore)));
                        }
                    }
                }

                if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Stage"))))
                {
                    LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("STAGE %d"), CurrentLevelIndex + 1)));
                }
            }
        }
    }
}

void AHighScoreGameState::OnGameOver()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (AHighScorePlayerController* HighScorePlayerController = Cast<AHighScorePlayerController>(PlayerController))
        {
            HighScorePlayerController->SetPause(true);
            HighScorePlayerController->ShowMainMenu(true);
        }
    }
}
