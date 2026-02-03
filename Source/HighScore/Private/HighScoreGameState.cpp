// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#include "HighScoreGameState.h"
#include "Character/HighScorePlayerController.h"
#include "Character/HighScoreCharacter.h"
#include "HighScoreGameInstance.h"
#include "HighScoreHUD.h"
#include "Item/SpawnVolume.h"
#include "Item/CoinItem.h"
#include "Kismet/GameplayStatics.h"

AHighScoreGameState::AHighScoreGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    MaxLevel = 3;
    CurrentLevelIndex = 0;
    MaxWave = 3;
    CurrentWaveIndex = 1;

    MaxLevelDuration = 60.0f;
    LevelDuration = MaxLevelDuration;
    
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

void AHighScoreGameState::AddScore(int32 Amount)
{
    if (UHighScoreGameInstance* HighScoreGI = Cast<UHighScoreGameInstance>(GetGameInstance()))
    {
        HighScoreGI->AddToScore(Amount);
    }
}

void AHighScoreGameState::StartLevel()
{
    // GameUI 표시
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (AHighScorePlayerController* HighScorePC = Cast<AHighScorePlayerController>(PC))
        {
            HighScorePC->ShowMainMenuHUD(false);
            HighScorePC->ShowGameplayHUD();
        }
    }

    // 현재 맵 레벨 설정
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UHighScoreGameInstance* HighScoreGI = Cast<UHighScoreGameInstance>(GI))
        {
            CurrentLevelIndex = HighScoreGI->CurrentLevelIndex;
        }
    }

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &AHighScoreGameState::UpdateGameplayHUD,
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

    LevelDuration = MaxLevelDuration - ((CurrentWaveIndex - 1) * MultipleTime);
    int32 CurrentWaveItemCount = ItemToSpawn - ((CurrentWaveIndex - 1) * MultipleItem);

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
    if (FoundVolumes.Num() > 0)
    {
        if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]))
        {
            // [수정 핵심] 루프 스폰을 제거하고 배치 스폰 함수 한 번만 호출
            TArray<AActor*> SpawnedItems = SpawnVolume->SpawnMultipleItems(CurrentWaveItemCount);

            // 생성된 아이템 중 코인이 몇 개인지만 체크
            for (AActor* Item : SpawnedItems)
            {
                if (Item && Item->IsA(ACoinItem::StaticClass()))
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
    if (CurrentWaveIndex < MaxWave)
    {
        // 다음 웨이브 진행
        CurrentWaveIndex++;
        StartWave();
        UpdateGameplayHUD();
    }
    else
    {
        // 3웨이브까지 끝났다면 다음 레벨로 이동
        CurrentWaveIndex = 1;
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

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UHighScoreGameInstance* HighScoreGI = Cast<UHighScoreGameInstance>(GI))
        {
            AddScore(Score);
            CurrentLevelIndex++;
            HighScoreGI->CurrentLevelIndex = CurrentLevelIndex;

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

void AHighScoreGameState::UpdateGameplayHUD()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    AHighScoreHUD* HUDWidget = Cast<AHighScoreHUD>(PC->GetHUD());
    AHighScoreCharacter* HighScoreCharacter = Cast<AHighScoreCharacter>(PC->GetPawn());

    if (HUDWidget && HighScoreCharacter)
    {
        float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
        int32 TotalScore = 0;
        if (UHighScoreGameInstance* GI = Cast<UHighScoreGameInstance>(GetGameInstance()))
        {
            TotalScore = GI->TotalScore;
        }

        // HUD에 데이터 전달
        HUDWidget->UpdateGameplayHUD(
            HighScoreCharacter->GetHealth(),
            HighScoreCharacter->GetMaxHealth(),
            RemainingTime,
            LevelDuration,
            TotalScore,
            CurrentLevelIndex + 1,
            CurrentWaveIndex,
            SpawnedCoinCount,
            CollectedCoinCount
        );
    }
}

void AHighScoreGameState::OnGameOver()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    AHighScorePlayerController* HighScorePC = Cast<AHighScorePlayerController>(PC);
    if (HighScorePC)
    {
        HighScorePC->ShowMainMenuHUD(true);
        HighScorePC->SetIsExit(false);
        PC->SetIgnoreLookInput(true);
        PC->SetIgnoreMoveInput(true);
        GetWorldTimerManager().ClearTimer(LevelTimerHandle);
    }
}
