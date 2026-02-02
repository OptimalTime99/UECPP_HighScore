// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
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

    UPROPERTY(Category = "HUD", VisibleAnywhere)
    TObjectPtr<UUserWidget> GameplayWidgetInstance;

    UPROPERTY(Category = "HUD", VisibleAnywhere)
    TObjectPtr<UUserWidget> MainMenuWidgetInstance;

    UFUNCTION(Category = "HUD", BlueprintCallable)
    void ShowMainMenuHUD(bool bIsRestart);

    UFUNCTION(Category = "HUD", BlueprintCallable)
    void ShowGameplayHUD();

    UFUNCTION(Category = "HUD", BlueprintPure)
    UUserWidget* GetGameplayWidget() const { return GameplayWidgetInstance; }

    UFUNCTION(Category = "HUD", BlueprintCallable)
    void UpdateMainMenuHUD(bool bIsRestart);

    UFUNCTION(Category = "HUD", BlueprintCallable)
    void UpdateGameplayHUD(float Health, float MaxHealth, float RemainingTime, float Duration, int32 Score, int32 Level, int32 Wave, int SpawnedCoinCount, int CollectedCoinCount);

    UFUNCTION(Category = "HUD", BlueprintCallable)
    void ClearAllWidgets();
};
