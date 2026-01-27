// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "HealingItem.generated.h"

UCLASS()
class HIGHSCORE_API AHealingItem : public ABaseItem
{
    GENERATED_BODY()
    
public:
    AHealingItem();

    virtual void ActivateItem(AActor* Activator) override;

private:
    UPROPERTY(Category = "Item", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float HealAmount;
};
