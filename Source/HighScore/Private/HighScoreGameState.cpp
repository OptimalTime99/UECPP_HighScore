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
#include "HighScoreHUD.h"

AHighScoreGameState::AHighScoreGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    CurrentLevelIndex = 0;
    MaxLevels = 3;
    MaxLevelDuration = 60.0f;
    LevelDuration = MaxLevelDuration;
    LevelWave = 1;
    ItemToSpawn = 40;
    MultipleTime = 15;
    MultipleItem = 10;
}

void AHighScoreGameState::BeginPlay()
{
    Super::BeginPlay();

    // 게임 시작 시 첫 레벨부터 진행
    StartLevel();
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
    // GameUI 표시
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (AHighScorePlayerController* HighScorePlayerController = Cast<AHighScorePlayerController>(PlayerController))
        {
            HighScorePlayerController->ShowMainMenuHUD(false);
            HighScorePlayerController->ShowGameplayHUD();
        }
    }

    // 현재 맵 레벨 설정
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UHighScoreGameInstance* HighScoreGameInstance = Cast<UHighScoreGameInstance>(GameInstance);
        if (HighScoreGameInstance)
        {
            CurrentLevelIndex = HighScoreGameInstance->CurrentLevelIndex;
        }
    }

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &AHighScoreGameState::UpdateHUD,
        0.1f,
        true
    );

    StartWave();
}

void AHighScoreGameState::StartWave()
{
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    GetWorldTimerManager().ClearTimer(LevelTimerHandle);

    LevelDuration = MaxLevelDuration - ((LevelWave - 1) * MultipleTime);    
    int32 CurrentWaveItemCount = ItemToSpawn - ((LevelWave - 1) * MultipleItem);

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
    if (FoundVolumes.Num() > 0)
    {
        ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
        if (SpawnVolume)
        {
            for (int32 i = 0; i < CurrentWaveItemCount; i++)
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
    if (LevelWave < 3)
    {
        // 다음 웨이브 진행
        LevelWave++;
        StartWave();
        UpdateHUD();
    }
    else
    {
        // 3웨이브까지 끝났다면 다음 레벨로 이동
        LevelWave = 1; 
        EndLevel();
    }
}

void AHighScoreGameState::OnCoinCollected()
{
    CollectedCoinCount++;

    // 현재 레벨에서 스폰된 코인을 전부 주웠다면 즉시 레벨 종료
    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
    {
        // 타이머를 멈추고 강제로 OnLevelTimeUp을 호출하여 다음 단계로 넘김
        GetWorldTimerManager().ClearTimer(LevelTimerHandle);
        OnLevelTimeUp();
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
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    AHighScoreHUD* HUDWidget = Cast<AHighScoreHUD>(PC->GetHUD());
    AHighScoreCharacter* MyChar = Cast<AHighScoreCharacter>(PC->GetPawn());

    if (HUDWidget && MyChar)
    {
        float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
        int32 TotalScore = 0;
        if (UHighScoreGameInstance* GI = Cast<UHighScoreGameInstance>(GetGameInstance()))
        {
            TotalScore = GI->TotalScore;
        }

        // HUD에 데이터 전달
        HUDWidget->UpdateHUDContents(
            MyChar->GetHealth(),
            MyChar->GetMaxHealth(),
            RemainingTime,
            LevelDuration,
            TotalScore,
            CurrentLevelIndex + 1,
            LevelWave
        );
    }
}

void AHighScoreGameState::OnGameOver()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("OnGameOver() 실행"));
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    AHighScorePlayerController* HPC = Cast<AHighScorePlayerController>(PC);
    if (HPC)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("OnGameOver()에 HPC가 있음"));
        HPC->ShowMainMenuHUD(true);
        PC->SetIgnoreLookInput(true);
        PC->SetIgnoreMoveInput(true);
        HPC->SetIsExit(false);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("OnGameOver()에 HPC가 없음"));
    }
}
