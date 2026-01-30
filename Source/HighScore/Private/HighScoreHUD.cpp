// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "HighScoreHUD.h"
#include "Components/TextBlock.h"
#include "HighScoreGameInstance.h"
#include "Components/ProgressBar.h"


void AHighScoreHUD::ShowGameplayHUD()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("1. ShowGameplayHUD 실행"));

    if (GameplayWidgetClass)
    {
        ClearAllWidgets();
        APlayerController* PC = GetOwningPlayerController();
        HUDWidgetInstance = CreateWidget<UUserWidget>(GetOwningPlayerController(), GameplayWidgetClass);
        if (PC && HUDWidgetInstance)
        {
            HUDWidgetInstance->AddToViewport();

            // 게임 모드로 전환
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("2 HUDWidgetInstance가 없음"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("3 GameplayWidgetClass가 없음"));
    }
}

void AHighScoreHUD::ShowMainMenuHUD(bool bIsRestart)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("1. ShowMainMenuHUD 실행"));

    if (MainMenuWidgetClass)
    {
        ClearAllWidgets();
        APlayerController* PC = GetOwningPlayerController();
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
        if (PC && MainMenuWidgetInstance)
        {
            MainMenuWidgetInstance->AddToViewport();
            UpdateMainMenuUI(bIsRestart);

            // UI 모드로 전환
            FInputModeUIOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("2 MainMenuWidgetInstance가 없음"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("3 MainMenuWidgetClass가 없음"));
    }
}

void AHighScoreHUD::UpdateMainMenuUI(bool bIsRestart)
{
    if (!MainMenuWidgetInstance) return;

    // Start 버튼 텍스트 변경
    if (UTextBlock* StartButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
    {
        StartButtonText->SetText(FText::FromString(bIsRestart ? TEXT("RESTART") : TEXT("START")));
    }

    // Exit 버튼 텍스트 변경
    if (UTextBlock* ExitButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("ExitButtonText"))))
    {
        ExitButtonText->SetText(FText::FromString(bIsRestart ? TEXT("MAIN MENU") : TEXT("EXIT")));
    }

    if (bIsRestart)
    {
        // 게임오버 애니메이션 실행 (블루프린트 함수 호출)
        UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
        if (PlayAnimFunc)
        {
            MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
        }

        // 점수 표시
        if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
        {
            if (UHighScoreGameInstance* GI = Cast<UHighScoreGameInstance>(GetGameInstance()))
            {
                TotalScoreText->SetText(FText::Format(FText::FromString("Total Score: {0}"), FText::AsNumber(GI->TotalScore)));
            }
        }
    }
}

void AHighScoreHUD::ClearAllWidgets()
{
    // 메인 메뉴 위젯이 떠 있다면 제거
    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    // 게임 HUD 위젯이 떠 있다면 제거
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }
}

void AHighScoreHUD::UpdateHUDContents(float Health, float MaxHealth, float RemainingTime, float Duration, int32 Score, int32 Level, int32 Wave)
{
    if (!HUDWidgetInstance) return;

    // HP 업데이트
    if (UTextBlock* HPText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("HP"))))
        HPText->SetText(FText::AsNumber((int32)Health));

    if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidgetInstance->GetWidgetFromName(TEXT("PB_HP"))))
        HPBar->SetPercent(MaxHealth > 0 ? Health / MaxHealth : 0);

    // 시간 업데이트
    if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("Time"))))
        TimeText->SetText(FText::Format(FText::FromString("{0}"), FText::AsNumber(FMath::Max(RemainingTime, 0.0f), &FNumberFormattingOptions().SetMaximumFractionalDigits(1))));

    if (UProgressBar* TimeBar = Cast<UProgressBar>(HUDWidgetInstance->GetWidgetFromName(TEXT("PB_Time"))))
        TimeBar->SetPercent(Duration > 0 ? FMath::Clamp(RemainingTime / Duration, 0.0f, 1.0f) : 0);

    // 점수 및 스테이지
    if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("Score"))))
        ScoreText->SetText(FText::Format(FText::FromString("SCORE: {0}"), FText::AsNumber(Score)));

    if (UTextBlock* LevelText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("Stage"))))
        LevelText->SetText(FText::Format(FText::FromString("STAGE {0}-{1}"), FText::AsNumber(Level), FText::AsNumber(Wave)));
}
