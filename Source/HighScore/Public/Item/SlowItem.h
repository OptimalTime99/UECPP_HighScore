// Copyright (c) 2026 Junhyeok Choi. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "SlowItem.generated.h"

class UGameplayEffect;

UCLASS()
class HIGHSCORE_API ASlowItem : public ABaseItem
{
    GENERATED_BODY()

public:
    ASlowItem();

protected:
    virtual void ActivateItem(AActor* Activator) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> SlowEffectClass;
};