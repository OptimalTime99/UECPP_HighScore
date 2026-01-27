// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // FTableRowBase 정의가 들어있는 헤더
#include "ItemSpawnRow.generated.h"


USTRUCT(BlueprintType)
struct FItemSpawnRow : public FTableRowBase
{
    GENERATED_BODY()

private:
    // 아이템 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FName ItemName;

    // 어떤 아이템 클래스를 스폰할지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> ItemClass;

    // 이 아이템의 스폰 확률
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float SpawnChance;
};
