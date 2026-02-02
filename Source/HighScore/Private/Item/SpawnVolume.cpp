// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

ASpawnVolume::ASpawnVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    // 박스 컴포넌트를 생성하고, 이 액터의 루트로 설정
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
    SpawningBox->SetupAttachment(Scene);

    ItemDataTable = nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
    if (FItemSpawnRow* SelectedRow = GetRandomItem())
    {
        if (UClass* ActualClass = SelectedRow->ItemClass.Get())
        {
            // 여기서 SpawnItem()을 호출하고, 스폰된 AActor 포인터를 리턴
            return SpawnItem(ActualClass);
        }
    }

    return nullptr;
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
    if (!ItemClass) return nullptr;

    // 1. 박스 범위 내 랜덤한 X, Y 좌표와 가장 높은 Z(천장) 좌표를 가져옵니다.
    FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
    FVector BoxOrigin = SpawningBox->GetComponentLocation();

    FVector StartLocation = BoxOrigin + FVector(
        FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
        FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
        BoxExtent.Z // 박스의 가장 윗부분에서 시작
    );

    // 2. 바닥 방향으로 쏠 끝점 (박스의 가장 아랫부분보다 더 아래까지 탐색)
    FVector EndLocation = StartLocation;
    EndLocation.Z -= (BoxExtent.Z * 2.0f + 500.0f);

    // 3. Line Trace 설정
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this); // 스폰 볼륨 자신은 무시

    FVector FinalSpawnLocation = StartLocation; // 기본값

    // 4. Trace 실행 (지면 탐색)
    if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
    {
        // 바닥에 부딪혔다면 그 지점의 위치를 사용
        FinalSpawnLocation = HitResult.Location;

        // [중요] 아이템이 바닥에 파묻히지 않게 약간 띄워줍니다 (Offset)
        // 아이템의 크기에 따라 10.0f ~ 50.0f 사이로 조절하세요.
        FinalSpawnLocation.Z += 100.0f;
    }

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        ItemClass,
        FinalSpawnLocation,
        FRotator::ZeroRotator
    );

    return SpawnedActor;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
    if (!ItemDataTable) return nullptr;

    // 1) 모든 Row(행) 가져오기
    TArray<FItemSpawnRow*> AllRows;
    static const FString ContextString(TEXT("ItemSpawnContext"));
    ItemDataTable->GetAllRows(ContextString, AllRows);

    if (AllRows.IsEmpty()) return nullptr;

    // 2) 전체 확률 합 구하기
    float TotalChance = 0.0f; // 초기화
    for (const FItemSpawnRow* Row : AllRows) // AllRows 배열의 각 Row를 순회
    {
        if (Row) // Row가 유효한지 확인
        {
            TotalChance += Row->SpawnChance; // SpawnChance 값을 TotalChance에 더하기
        }
    }

    // 3) 0 ~ TotalChance 사이 랜덤 값
    const float RandValue = FMath::FRandRange(0.0f, TotalChance);
    float AccumulateChance = 0.0f;

    // 4) 누적 확률로 아이템 선택
    for (FItemSpawnRow* Row : AllRows)
    {
        AccumulateChance += Row->SpawnChance;
        if (RandValue <= AccumulateChance)
        {
            return Row;
        }
    }

    return nullptr;
}
