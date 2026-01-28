// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "HighScoreGameInstance.h"

UHighScoreGameInstance::UHighScoreGameInstance()
{
    TotalScore = 0;
    CurrentLevelIndex = 0;
}

void UHighScoreGameInstance::AddToScore(int32 Amount)
{
    TotalScore += Amount;
}
