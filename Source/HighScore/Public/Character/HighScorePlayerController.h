// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HighScorePlayerController.generated.h"

class UInputMappingContext; // IMC 관련 전방 선언
class UInputAction; // IA 관련 전방 선언

/**
 * 
 */
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

#pragma region HUD
    // UMG 위젯 클래스를 에디터에서 할당받을 변수
    UPROPERTY(Category = "HUD", EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY(Category = "HUD", VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UUserWidget> HUDWidgetInstance;

    UFUNCTION(Category = "HUD", BlueprintPure)
    UUserWidget* GetHUDWidget() const;
#pragma endregion

protected:
    virtual void BeginPlay() override;
};
