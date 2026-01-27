// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/BigCoinItem.h"

ABigCoinItem::ABigCoinItem()
{
    PointValue = 50;
    ItemType = "BigCoin";
}

void ABigCoinItem::ActivateItem(AActor* Activator)
{
    DestroyItem(); // 아이템 소멸(부모의 DestroyItem() 혹은 자체 로직)
}
