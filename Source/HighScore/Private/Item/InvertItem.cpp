// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/InvertItem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "HighScoreGameState.h"

AInvertItem::AInvertItem()
{
    ItemType = "InvertControl";
}

void AInvertItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator)
    {
        IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Activator);
        if (ASCInterface && InvertEffectClass)
        {
            if (UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent())
            {
                // 스테이지 기반 레벨 계산 (지속 시간 결정을 위함)
                float EffectLevel = 1.0f;
                if (AHighScoreGameState* GS = GetWorld()->GetGameState<AHighScoreGameState>())
                {
                    EffectLevel = static_cast<float>((GS->GetCurrentLevelIndex() * 3) + GS->GetCurrentWaveIndex() + 1);
                }

                FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
                Context.AddInstigator(this, this);

                FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(InvertEffectClass, EffectLevel, Context);
                if (SpecHandle.IsValid())
                {
                    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                }
            }
        }

        DestroyItem();
    }
}
