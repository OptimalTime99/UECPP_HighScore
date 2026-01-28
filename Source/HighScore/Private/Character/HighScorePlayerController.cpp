// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Character/HighScorePlayerController.h"
#include "EnhancedInputSubsystems.h" // Enhanced Input System의 Local Player Subsystem을 사용하기 위해 포함
#include "Blueprint/UserWidget.h"
#include "HighScoreGameState.h"
#include "HighScoreGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

AHighScorePlayerController::AHighScorePlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr),
    HUDWidgetClass(nullptr),
    HUDWidgetInstance(nullptr),
    MainMenuWidgetClass(nullptr),
    MainMenuWidgetInstance(nullptr)
{
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

    // 게임 실행 시 메뉴 레벨에서 메뉴 UI 먼저 표시
    FString CurrentMapName = GetWorld()->GetMapName();
    if (CurrentMapName.Contains("L_MenuLevel"))
    {
        ShowMainMenu(false);
    }
}

UUserWidget* AHighScorePlayerController::GetHUDWidget() const
{
    return HUDWidgetInstance;
}

void AHighScorePlayerController::ShowGameHUD()
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

			AHighScoreGameState* HighScoreGameState = GetWorld() ? GetWorld()->GetGameState<AHighScoreGameState>() : nullptr;
			if (HighScoreGameState)
			{
				HighScoreGameState->UpdateHUD();
			}
		}
	}
}

void AHighScorePlayerController::ShowMainMenu(bool bIsRestart)
{
	// HUD가 켜져 있다면 닫기
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	// 메뉴 UI 생성
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
			{
				if (UHighScoreGameInstance* HighScoreGameInstance = Cast<UHighScoreGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Total Score: %d"), HighScoreGameInstance->TotalScore)
					));
				}
			}
		}
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