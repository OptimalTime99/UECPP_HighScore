// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/HealingItem.h"

AHealingItem::AHealingItem()
{
    HealAmount = 20.0f;
    ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
    // 플레이어 캐릭터의 체력을 20만큼 회복시키는 로직 등을 구현 가능
    DestroyItem();
}
