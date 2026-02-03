// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemComponent.h"
#include "HighScoreCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;

// Enhanced Input에서 액션 값을 받을 때 사용하는 구조체
struct FInputActionValue;

UCLASS()
class HIGHSCORE_API AHighScoreCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AHighScoreCharacter();

    virtual void PossessedBy(AController* NewController) override;
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    // 현재 체력을 가져오는 함수
    UFUNCTION(Category = "Health", BlueprintPure)
    int32 GetHealth() const;

    // 최대 체력을 가져오는 함수
    UFUNCTION(Category = "Health", BlueprintPure)
    int32 GetMaxHealth() const;


    // 사망 처리 함수 (체력이 0 이하가 되었을 때 호출)
    void OnDeath();

    void UpdateOverheadHP();

    void OnHealthChanged(const FOnAttributeChangeData& Data);

protected:
    // 2. ASC와 AttributeSet 멤버 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
    class UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    class UHighScoreAttributeSet* AttributeSet;

    // [추가] 에디터에서 GE_Sprint를 할당하기 위한 변수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> SprintEffectClass;

    // 3. 나중에 OnDeath 바인딩을 위한 함수
    void BindAttributeCallbacks();

protected:
#pragma region override
    virtual void BeginPlay() override;
    
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion

#pragma region action
    // IA_Move와 IA_Jump 등을 처리할 함수 원형
    // Enhanced Input에서 액션 값은 FInputActionValue로 전달됩니다.
    UFUNCTION()
    void Move(const FInputActionValue& value);
    UFUNCTION()
    void StartJump(const FInputActionValue& value);
    UFUNCTION()
    void StopJump(const FInputActionValue& value);
    UFUNCTION()
    void Look(const FInputActionValue& value);
    UFUNCTION()
    void StartSprint(const FInputActionValue& value);
    UFUNCTION()
    void StopSprint(const FInputActionValue& value);
#pragma endregion

private:
#pragma region Camera
    // 스프링 암 컴포넌트
    UPROPERTY(Category = "Camera", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> SpringArmComp;
    // 카메라 컴포넌트
    UPROPERTY(Category = "Camera", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> CameraComp;
#pragma endregion

    UPROPERTY(Category = "UI", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UWidgetComponent> OverheadWidget;

#pragma region Movement
    // 이동 속도 관련 프로퍼티들
    UPROPERTY(Category = "Movement", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float NormalSpeed; // 기본 걷기 속도
    UPROPERTY(Category = "Movement", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float SprintSpeedMultiplier;  // "기본 속도" 대비 몇 배로 빠르게 달릴지 결정
    UPROPERTY(Category = "Movement", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float SprintSpeed; // 실제 스프린트 속도
#pragma endregion

    // [추가] 스프린트를 멈출 때 GE를 제거하기 위해 핸들을 저장합니다.
    FActiveGameplayEffectHandle SprintEffectHandle;
};
