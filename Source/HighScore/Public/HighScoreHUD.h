// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "HighScoreHUD.generated.h"

UCLASS()
class HIGHSCORE_API AHighScoreHUD : public AHUD
{
    GENERATED_BODY()

public:
    UPROPERTY(Category = "HUD", EditAnywhere)
    TSubclassOf<UUserWidget> GameplayWidgetClass;

    UPROPERTY(Category = "HUD", EditAnywhere)
    TSubclassOf<UUserWidget> MainMenuWidgetClass;


    UPROPERTY()
    TObjectPtr<UUserWidget> HUDWidgetInstance;

    UPROPERTY()
    TObjectPtr<UUserWidget> MainMenuWidgetInstance;


    UFUNCTION(Category = "HUD", BlueprintCallable)
    void ShowGameplayHUD();

    UFUNCTION(Category = "HUD", BlueprintCallable)
    void ShowMainMenuHUD(bool bIsRestart);

    void UpdateMainMenuUI(bool bIsRestart);

    void ClearAllWidgets();

    UFUNCTION(BlueprintPure, Category = "HUD")
    UUserWidget* GetHUDWidget() const { return HUDWidgetInstance; }

    void UpdateHUDContents(float Health, float MaxHealth, float RemainingTime, float Duration, int32 Score, int32 Level, int32 Wave);
};
