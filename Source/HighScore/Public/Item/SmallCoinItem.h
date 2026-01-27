// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/CoinItem.h"
#include "SmallCoinItem.generated.h"

UCLASS()
class HIGHSCORE_API ASmallCoinItem : public ACoinItem
{
    GENERATED_BODY()
    
public:
    ASmallCoinItem();

    virtual void ActivateItem(AActor* Activator) override;
};
