// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HighScorePlayerController.generated.h"

class UInputMappingContext; // IMC 관련 전방 선언
class UInputAction; // IA 관련 전방 선언

UCLASS()
class HIGHSCORE_API AHighScorePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AHighScorePlayerController();

#pragma region Input
    // 에디터에서 세팅할 IMC
    UPROPERTY(Category = "Input", EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputMappingContext> InputMappingContext;

    // IA_Move를 지정할 변수
    UPROPERTY(Category = "Input", EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> MoveAction;

    // IA_Jump를 지정할 변수
    UPROPERTY(Category = "Input", EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> JumpAction;

    // IA_Look를 지정할 변수
    UPROPERTY(Category = "Input", EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> LookAction;

    // IA_Sprint를 지정할 변수
    UPROPERTY(Category = "Input", EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UInputAction> SprintAction;
#pragma endregion


#pragma region Menu
    // 게임 시작
    UFUNCTION(Category = "Menu", BlueprintCallable)
    void StartGame();

    // 게임 종료
    UFUNCTION(Category = "Menu", BlueprintCallable)
    void ExitGame();

    // 게임 플레이 화면 표시
    UFUNCTION(Category = "HUD", BlueprintCallable)
    void ShowGameplayHUD();

    // 메인 메뉴 표시
    UFUNCTION(Category = "Menu", BlueprintCallable)
    void ShowMainMenuHUD(bool bIsRestart);

    void UpdateMainMenuUI(bool bIsRestart);

    void SetIsExit(bool bIsExit);

#pragma endregion

protected:
    virtual void BeginPlay() override;

private:
    bool bIsExit;
};
