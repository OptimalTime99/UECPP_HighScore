// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#include "Character/HighScorePlayerController.h"
#include "EnhancedInputSubsystems.h" // Enhanced Input System의 Local Player Subsystem을 사용하기 위해 포함
#include "HighScoreGameState.h"
#include "HighScoreGameInstance.h"
#include "HighScoreHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AHighScorePlayerController::AHighScorePlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr)
{
    bIsExit = true;
}

void AHighScorePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }
}

void AHighScorePlayerController::ShowGameplayHUD()
{
    if (AHighScoreHUD* HUDWidget = GetHUD<AHighScoreHUD>())
    {
        HUDWidget->ShowGameplayHUD();

        if (AHighScoreGameState* HighScoreGS = GetWorld()->GetGameState<AHighScoreGameState>())
        {
            HighScoreGS->UpdateGameplayHUD();
        }
    }
}

void AHighScorePlayerController::ShowMainMenuHUD(bool bIsRestart)
{
    if (AHighScoreHUD* HUDWidget = GetHUD<AHighScoreHUD>())
    {
        HUDWidget->ShowMainMenuHUD(bIsRestart);
        SetPause(false);
    }
}

void AHighScorePlayerController::StartGame()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("StartGame Called!"));

    if (UHighScoreGameInstance* HighScoreGI = Cast<UHighScoreGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        HighScoreGI->CurrentLevelIndex = 0;
        HighScoreGI->TotalScore = 0;
    }

    UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/Maps/L_Basic"));
}

void AHighScorePlayerController::ExitGame()
{
    if (bIsExit)
    {
        // EXIT 버튼 누르면 게임 종료
        UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
    }
    else
    {
        // MAIN MENU 버튼 누르면 메인 메뉴로 이동
        UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/Maps/L_MenuLevel"));
        ShowMainMenuHUD(false);
    }
}
