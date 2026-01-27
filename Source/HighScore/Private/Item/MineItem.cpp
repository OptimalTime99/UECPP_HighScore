// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/MineItem.h"

AMineItem::AMineItem()
{
    ExplosionDelay = 5.0f;
    ExplosionRadius = 300.0f;
    ExplosionDamage = 30.0f;
    ItemType = "Mine";
}

void AMineItem::ActivateItem(AActor* Activator)
{
    // 지뢰를 "사용"한다고 가정했을 때의 로직
    // 여기서는 간단히 소멸시키지만,
    // 실제로는 지연 시간 후 폭발 로직을 구현하거나,
    // 폭발 이펙트, 데미지 계산 등을 추가할 수 있음
    DestroyItem();
}
