// Copyright (c) 2026 Junhyeok Choi. All rights reserved.
#include "HighScoreGameMode.h"
#include "HighScoreCharacter.h"
#include "HighScorePlayerController.h"

AHighScoreGameMode::AHighScoreGameMode()
{
	DefaultPawnClass = AHighScoreCharacter::StaticClass();
	
	PlayerControllerClass = AHighScorePlayerController::StaticClass();
}
