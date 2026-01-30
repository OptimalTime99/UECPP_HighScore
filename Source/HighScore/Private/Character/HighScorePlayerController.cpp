// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Character/HighScorePlayerController.h"
#include "EnhancedInputSubsystems.h" // Enhanced Input System의 Local Player Subsystem을 사용하기 위해 포함
#include "Blueprint/UserWidget.h"
#include "HighScoreGameState.h"
#include "HighScoreGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HighScoreHUD.h"

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

    // 현재 PlayerController에 연결된 Local Player 객체를 가져옴    
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        // Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                // Subsystem을 통해 우리가 할당한 IMC를 활성화
                // 우선순위(Priority)는 0이 가장 높은 우선순위
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
        }
    }

    if (GetWorld()->GetMapName().Contains(TEXT("L_MenuLevel")))
    {
        bShowMouseCursor = true;
        bEnableClickEvents = true;
        bEnableMouseOverEvents = true;
    }
}

void AHighScorePlayerController::ShowGameplayHUD()
{
    if (AHighScoreHUD* HUDWidget = GetHUD<AHighScoreHUD>())
    {
        HUDWidget->ShowGameplayHUD();

        if (AHighScoreGameState* HighScoreGameState = GetWorld()->GetGameState<AHighScoreGameState>())
        {
            HighScoreGameState->UpdateHUD();
        }
    }
}

void AHighScorePlayerController::ShowMainMenuHUD(bool bIsRestart)
{
    // HUD를 가져와서 명령 전달
    if (AHighScoreHUD* HUDWidget = GetHUD<AHighScoreHUD>())
    {
        HUDWidget->ShowMainMenuHUD(bIsRestart);
    }
}

void AHighScorePlayerController::StartGame()
{
    if (UHighScoreGameInstance* HighScoreGameInstance = Cast<UHighScoreGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        HighScoreGameInstance->CurrentLevelIndex = 0;
        HighScoreGameInstance->TotalScore = 0;
    }

    UGameplayStatics::OpenLevel(GetWorld(), FName("L_Basic"));
    SetPause(false);
}

void AHighScorePlayerController::ExitGame()
{
    if (bIsExit)
    {
        // 평상시엔 게임 종료
        UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
    }
    else
    {
        // 죽었을 때는 메인 메뉴로 이동
        UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_MenuLevel"));
        ShowMainMenuHUD(false);
    }
}

void AHighScorePlayerController::SetIsExit(bool bInIsExit)
{
    bIsExit = bInIsExit;
}
