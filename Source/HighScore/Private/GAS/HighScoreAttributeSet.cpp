// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "GAS/HighScoreAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UHighScoreAttributeSet::UHighScoreAttributeSet()
{
    // 초기값 설정
    InitHealth(50.0f);
    InitMaxHealth(100.0f);
    InitMoveSpeed(600.0f);
}

void UHighScoreAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Health, MaxHealth, MoveSpeed 네트워크 복제 등록
    DOREPLIFETIME_CONDITION_NOTIFY(UHighScoreAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UHighScoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UHighScoreAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UHighScoreAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UHighScoreAttributeSet, Health, OldHealth);
}

void UHighScoreAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UHighScoreAttributeSet, MaxHealth, OldMaxHealth);
}

void UHighScoreAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UHighScoreAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UHighScoreAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // Health를 0 ~ MaxHealth로 Clamp
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
}

void UHighScoreAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    // 1. MaxHealth가 변했을 때 현재 Health 비율 유지
    if (Attribute == GetMaxHealthAttribute())
    {
        AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
    }

    // 2. MoveSpeed가 변했을 때 실제 CharacterMovementComponent에 적용
    if (Attribute == GetMoveSpeedAttribute())
    {
        // 1. ASC로부터 아바타 액터(실제 월드에 존재하는 몸체)를 가져옴
        AActor* TargetActor = GetOwningAbilitySystemComponent()->GetAvatarActor();

        // 2. 해당 액터가 캐릭터인지 확인 (ACharacter로 캐스팅)
        if (ACharacter* Character = Cast<ACharacter>(TargetActor))
        {
            // 3. 캐릭터 무브먼트 컴포넌트에 접근하여 값 수정
            if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
            {
                Character->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
            }
        }
    }
}

void UHighScoreAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // GE를 통해 최종 계산된 결과값 클램핑
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

        if (GetHealth() <= 0.0f)
        {
            // TODO: 여기에 사망 처리 브로드캐스트 로직 추가 (예: OnDeath.Broadcast())
            // 가해자 정보 찾기 (Data.EffectSpec에서 추출)
            AActor* Instigator = Data.EffectSpec.GetContext().GetInstigator();

            // 델리게이트 실행
            if (OnDeath.IsBound())
            {
                OnDeath.Broadcast(Instigator);
            }
        }
    }

    if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
    {
        // 1. 캐릭터(AvatarActor)를 가져옵니다.
        AActor* TargetActor = Data.Target.GetAvatarActor();
        if (ACharacter* Character = Cast<ACharacter>(TargetActor))
        {
            // 2. 변경된 MoveSpeed 값을 실제 이동 컴포넌트에 적용합니다.
            Character->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
        }
    }
}

void UHighScoreAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
    UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
    const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
    if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
    {
        // 최대치가 늘어난 만큼 현재치도 비율대로 조정
        const float CurrentValue = AffectedAttribute.GetCurrentValue();
        float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

        AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
    }
}
