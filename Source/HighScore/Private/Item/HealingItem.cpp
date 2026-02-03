// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/HealingItem.h"
#include "Character/HighScoreCharacter.h"
#include "HighScoreGameState.h"

AHealingItem::AHealingItem()
{
    ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator)
    {
        // 2. Activator가 GAS 인터페이스를 구현했는지 확인
        IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Activator);
        if (ASCInterface && HealingEffectClass)
        {
            UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
            if (TargetASC)
            {
                // 3. 현재 스테이지/웨이브 기반 레벨 계산
                float EffectLevel = 1.0f;
                if (AHighScoreGameState* GS = GetWorld()->GetGameState<AHighScoreGameState>())
                {
                    // 공식: ((Stage - 1) * 3) + Wave
                    EffectLevel = static_cast<float>(GS->GetCurrentLevelIndex() * 3 + GS->GetCurrentWaveIndex());
                }

                // 4. GE 적용을 위한 Context 및 Spec 생성
                FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
                ContextHandle.AddInstigator(this, this);

                FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(HealingEffectClass, EffectLevel, ContextHandle);
                if (SpecHandle.IsValid())
                {
                    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                }
            }
        }

        // 5. 아이템 제거 (BaseItem의 DestroyItem 호출)
        DestroyItem();
    }
}
