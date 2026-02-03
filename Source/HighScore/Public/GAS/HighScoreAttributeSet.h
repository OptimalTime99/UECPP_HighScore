// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HighScoreAttributeSet.generated.h"

// Attribute Accessors 매크로
#define ATTRIBUTE_ACCESSORS(ClassName,PropertyName)\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttributeSetDeathDelegate, AActor* /* 데미지 입힌 가해자 */);

/**
 * MyAttributeSet
 * Health, MaxHealth, MoveSpeed Attribute를 관리하는 AttributeSet
 */
UCLASS()
class HIGHSCORE_API UHighScoreAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UHighScoreAttributeSet();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ============================================================
    // Attributes
    // ============================================================
    
    // Health - 현재 체력
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UHighScoreAttributeSet, Health)

    // MaxHealth - 최대 체력
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UHighScoreAttributeSet, MaxHealth)

    // MoveSpeed - 이동 속도
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UHighScoreAttributeSet, MoveSpeed)

    // ============================================================
    // Replication Callbacks
    // ============================================================

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

    // ============================================================
    // Attribute Clamping
    // ============================================================

    // Attribute 변경 전 호출 (Clamping)
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    // Attribute 변경 후 호출
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


    // 사망 시 호출될 델리게이트
    FOnAttributeSetDeathDelegate OnDeath;

private:
    // 속성 변경 후 캐릭터 컴포넌트에 수치를 적용하는 헬퍼 함수
    void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
    
};
