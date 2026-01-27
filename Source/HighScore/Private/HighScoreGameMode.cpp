// Copyright (c) 2026 Junhyeok Choi. All rights reserved.
#include "HighScoreGameMode.h"
#include "Character/HighScoreCharacter.h"
#include "Character/HighScorePlayerController.h"
#include "HighScoreGameStateBase.h"

AHighScoreGameMode::AHighScoreGameMode()
{
	DefaultPawnClass = AHighScoreCharacter::StaticClass();
	
	PlayerControllerClass = AHighScorePlayerController::StaticClass();

	GameStateClass = AHighScoreGameStateBase::StaticClass();
}
