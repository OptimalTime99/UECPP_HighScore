// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "HighScoreGameState.generated.h"

UCLASS()
class HIGHSCORE_API AHighScoreGameState : public AGameState
{
    GENERATED_BODY()

public:
    AHighScoreGameState();

    virtual void BeginPlay() override;

    UPROPERTY(Category = "Score", VisibleAnyWhere, BlueprintReadWrite)
    int32 Score;

    // 현재 레벨에서 스폰된 코인 개수
    UPROPERTY(Category = "Coin", VisibleAnywhere, BlueprintReadOnly)
    int32 SpawnedCoinCount;

    // 플레이어가 수집한 코인 개수
    UPROPERTY(Category = "Coin", VisibleAnywhere, BlueprintReadOnly)
    int32 CollectedCoinCount;

    // 각 레벨이 유지되는 시간 (초 단위)
    UPROPERTY(Category = "Level", EditAnywhere, BlueprintReadOnly)
    float LevelDuration;

    // 현재 진행 중인 레벨 인덱스
    UPROPERTY(Category = "Level", VisibleAnywhere, BlueprintReadOnly)
    int32 CurrentLevelIndex;

    // 전체 레벨의 개수
    UPROPERTY(Category = "Level", VisibleAnywhere, BlueprintReadOnly)
    int32 MaxLevels;

    // 실제 레벨 맵 이름 배열. 여기 있는 인덱스를 차례대로 연동
    UPROPERTY(Category = "Level", EditAnywhere, BlueprintReadOnly)
    TArray<FName> LevelMapNames;

    int32 LevelWave;

    UPROPERTY(Category = "Level", EditAnywhere, BlueprintReadOnly)
    int32 ItemToSpawn;

    UPROPERTY(Category = "Level", EditAnywhere, BlueprintReadOnly)
    int32 MultipleTime;

    UPROPERTY(Category = "Level", EditAnywhere, BlueprintReadOnly)
    int32 MultipleItem;

    float MaxLevelDuration;

    // 매 레벨이 끝나기 전까지 시간이 흐르도록 관리하는 타이머
    FTimerHandle LevelTimerHandle;

    // UI Widget 시간 업데이트 타이머
    FTimerHandle HUDUpdateTimerHandle;

    UFUNCTION(Category = "Score", BlueprintPure)
    int32 GetScore() const;

    UFUNCTION(Category = "Score", BlueprintCallable)
    void AddScore(int32 Amount);

    // 게임이 완전히 끝났을 때 (모든 레벨 종료) 실행되는 함수
    UFUNCTION(Category = "Level", BlueprintCallable)
    void OnGameOver();

    // 레벨을 시작할 때, 아이템 스폰 및 타이머 설정
    void StartLevel();
    // 웨이브 시작할 때, 아이템 스폰 및 타이머 설정
    void StartWave();
    // 레벨 제한 시간이 만료되었을 때 호출
    void OnLevelTimeUp();
    // 코인을 주웠을 때 호출
    void OnCoinCollected();
    // 레벨을 강제 종료하고 다음 레벨로 이동
    void EndLevel();
    // 시간 초마다 업데이트
    void UpdateHUD();
    
    
};
