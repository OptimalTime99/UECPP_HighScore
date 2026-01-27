// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "HighScoreGameStateBase.h"

AHighScoreGameStateBase::AHighScoreGameStateBase()
{
    Score = 0;
}

int32 AHighScoreGameStateBase::GetScore() const
{
    return Score;
}

void AHighScoreGameStateBase::AddScore(int32 Amount)
{
    Score += Amount;
}