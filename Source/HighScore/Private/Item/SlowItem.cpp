// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/SlowItem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "HighScoreGameState.h"

ASlowItem::ASlowItem()
{
    ItemType = "Slow";
}

void ASlowItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator)
    {
        IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Activator);
        if (ASCInterface && SlowEffectClass)
        {
            if (UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent())
            {
                // 공식: (Stage * 3) + Wave + 1
                float EffectLevel = 1.0f;
                if (AHighScoreGameState* GS = GetWorld()->GetGameState<AHighScoreGameState>())
                {
                    EffectLevel = static_cast<float>((GS->GetCurrentLevelIndex() * 3) + GS->GetCurrentWaveIndex() + 1);
                }

                FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
                Context.AddInstigator(this, this);

                FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(SlowEffectClass, EffectLevel, Context);
                if (SpecHandle.IsValid())
                {
                    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                }
            }
        }

        DestroyItem();
    }
}

