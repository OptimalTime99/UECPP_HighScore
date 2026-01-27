// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "MineItem.generated.h"


UCLASS()
class HIGHSCORE_API AMineItem : public ABaseItem
{
    GENERATED_BODY()

public:
    AMineItem();

    virtual void ActivateItem(AActor* Activator) override;
    
private:
    // 폭발까지 걸리는 시간 (5초)
    UPROPERTY(Category = "Mine", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float ExplosionDelay;
    // 폭발 범위
    UPROPERTY(Category = "Mine", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float ExplosionRadius;
    // 폭발 데미지
    UPROPERTY(Category = "Mine", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float ExplosionDamage;

};
