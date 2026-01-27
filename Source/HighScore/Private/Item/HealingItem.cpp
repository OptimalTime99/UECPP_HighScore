// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/HealingItem.h"
#include "HighScoreCharacter.h"

AHealingItem::AHealingItem()
{
    HealAmount = 20.0f;
    ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (AHighScoreCharacter* PlayerCharacter = Cast<AHighScoreCharacter>(Activator))
        {
            // 캐릭터의 체력을 회복
            PlayerCharacter->AddHealth(HealAmount);
        }

        DestroyItem();
    }
}
