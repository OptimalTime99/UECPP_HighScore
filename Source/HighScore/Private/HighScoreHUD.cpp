// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#include "HighScoreHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HighScoreGameInstance.h"

void AHighScoreHUD::ShowGameplayHUD()
{
    if (GameplayWidgetClass)
    {
        ClearAllWidgets();
        APlayerController* PC = GetOwningPlayerController();
        GameplayWidgetInstance = CreateWidget<UUserWidget>(PC, GameplayWidgetClass);
        if (PC && GameplayWidgetInstance)
        {
            GameplayWidgetInstance->AddToViewport();

            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        }
    }
}

void AHighScoreHUD::ShowMainMenuHUD(bool bIsRestart)
{
    if (MainMenuWidgetClass)
    {
        ClearAllWidgets();
        APlayerController* PC = GetOwningPlayerController();
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
        if (PC && MainMenuWidgetInstance)
        {
            UpdateMainMenuHUD(bIsRestart);
            MainMenuWidgetInstance->AddToViewport();

            FInputModeUIOnly InputMode;
            PC->SetInputMode(InputMode);
        }
    }
}

void AHighScoreHUD::UpdateMainMenuHUD(bool bIsRestart)
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
    if (GameplayWidgetInstance)
    {
        GameplayWidgetInstance->RemoveFromParent();
        GameplayWidgetInstance = nullptr;
    }
}

void AHighScoreHUD::UpdateGameplayHUD(float Health, float MaxHealth, float RemainingTime, float Duration, int32 Score, int32 Level, int32 Wave, int SpawnedCoinCount, int CollectedCoinCount)
{
    if (!GameplayWidgetInstance) return;

    // HP 업데이트
    if (UTextBlock* HPText = Cast<UTextBlock>(GameplayWidgetInstance->GetWidgetFromName(TEXT("HP"))))
        HPText->SetText(FText::AsNumber((int32)Health));

    if (UProgressBar* HPBar = Cast<UProgressBar>(GameplayWidgetInstance->GetWidgetFromName(TEXT("PB_HP"))))
        HPBar->SetPercent(MaxHealth > 0 ? Health / MaxHealth : 0);

    // 시간 업데이트
    if (UTextBlock* TimeText = Cast<UTextBlock>(GameplayWidgetInstance->GetWidgetFromName(TEXT("Time"))))
        TimeText->SetText(FText::Format(FText::FromString("{0}"), FText::AsNumber(FMath::Max(RemainingTime, 0.0f), &FNumberFormattingOptions().SetMaximumFractionalDigits(1))));

    if (UProgressBar* TimeBar = Cast<UProgressBar>(GameplayWidgetInstance->GetWidgetFromName(TEXT("PB_Time"))))
        TimeBar->SetPercent(Duration > 0 ? FMath::Clamp(RemainingTime / Duration, 0.0f, 1.0f) : 0);

    // 점수 업데이트
    if (UTextBlock* ScoreText = Cast<UTextBlock>(GameplayWidgetInstance->GetWidgetFromName(TEXT("Score"))))
        ScoreText->SetText(FText::Format(FText::FromString("SCORE: {0}"), FText::AsNumber(Score)));

    // 스테이지 업데이트
    if (UTextBlock* LevelText = Cast<UTextBlock>(GameplayWidgetInstance->GetWidgetFromName(TEXT("Stage"))))
        LevelText->SetText(FText::Format(FText::FromString("STAGE {0}-{1}"), FText::AsNumber(Level), FText::AsNumber(Wave)));

    // 개수 업데이트
    if (UTextBlock* CoinCountText = Cast<UTextBlock>(GameplayWidgetInstance->GetWidgetFromName(TEXT("CoinCount"))))
        CoinCountText->SetText(FText::Format(FText::FromString("Coin            {0} / {1}"), FText::AsNumber(CollectedCoinCount), FText::AsNumber(SpawnedCoinCount)));
}
