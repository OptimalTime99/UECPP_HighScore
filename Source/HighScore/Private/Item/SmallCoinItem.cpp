// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/SmallCoinItem.h"

ASmallCoinItem::ASmallCoinItem()
{
    PointValue = 10;
    ItemType = "SmallCoin";
}

void ASmallCoinItem::ActivateItem(AActor* Activator)
{
    DestroyItem();
}
