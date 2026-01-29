// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HighScoreGameInstance.generated.h"


UCLASS()
class HIGHSCORE_API UHighScoreGameInstance : public UGameInstance
{
    GENERATED_BODY()
    
public:
    UHighScoreGameInstance();

    // 게임 전체 누적 점수
    UPROPERTY(Category = "GameData", VisibleAnywhere, BlueprintReadWrite)
    int32 TotalScore;
    // 현재 레벨 인덱스 (GameState에서도 관리할 수 있지만, 맵 전환 후에도 살리고 싶다면 GameInstance에 복제할 수 있음)
    UPROPERTY(Category = "GameData", VisibleAnywhere, BlueprintReadWrite)
    int32 CurrentLevelIndex;

    UFUNCTION(Category = "GameData", BlueprintCallable)
    void AddToScore(int32 Amount);
};
