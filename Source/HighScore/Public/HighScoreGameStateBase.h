// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HighScoreGameStateBase.generated.h"

UCLASS()
class HIGHSCORE_API AHighScoreGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
    AHighScoreGameStateBase();

    // 전역 점수를 저장하는 변수
    UPROPERTY(Category = "Score", VisibleAnywhere, BlueprintReadWrite)
    int32 Score;

    // 현재 점수를 읽는 함수
    UFUNCTION(Category = "Score", BlueprintPure)
    int32 GetScore() const;

    // 점수를 추가해주는 함수
    UFUNCTION(Category = "Score", BlueprintCallable)
    void AddScore(int32 Amount);
};
