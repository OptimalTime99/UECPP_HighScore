// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Maps/MazeGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/RandomShuffle.h"

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

FVector ASpawnVolume::GetRandomPointInVolume() const
{
    // 1) 박스 컴포넌트의 스케일된 Extent, 즉 x/y/z 방향으로 반지름(절반 길이)을 구함
    FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
    // 2) 박스 중심 위치
    FVector BoxOrigin = SpawningBox->GetComponentLocation();

    // 3) 각 축별로 -Extent ~ +Extent 범위의 무작위 값 생성
    return BoxOrigin + FVector(
        FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
        FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
        FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
    );
}

// [신규 구현] 중복 없는 일괄 스폰 함수
TArray<AActor*> ASpawnVolume::SpawnMultipleItems(int32 Count)
{
    TArray<AActor*> SpawnedActors; // 스폰된 액터들을 담을 배열
    if (Count <= 0) return SpawnedActors;

    AActor* MazeGenActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMazeGenerator::StaticClass());
    AMazeGenerator* MazeGen = Cast<AMazeGenerator>(MazeGenActor);
    if (!MazeGen) return SpawnedActors;

    TArray<FVector> Locations = MazeGen->GetMazePathLocations();
    if (Locations.IsEmpty()) return SpawnedActors;

    Algo::RandomShuffle(Locations);

    int32 ActualSpawnCount = FMath::Min(Count, Locations.Num());

    for (int32 i = 0; i < ActualSpawnCount; i++)
    {
        if (FItemSpawnRow* SelectedRow = GetRandomItem())
        {
            if (UClass* ActualClass = SelectedRow->ItemClass.Get())
            {
                // 생성된 액터를 배열에 추가
                AActor* NewItem = SpawnItem(ActualClass, Locations[i]);
                if (NewItem)
                {
                    SpawnedActors.Add(NewItem);
                }
            }
        }
    }

    return SpawnedActors; // 생성된 모든 아이템 리스트 반환
}

// [수정] 특정 위치를 받아서 물리 보정 후 스폰만 수행하는 함수
AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass, const FVector& InLocation)
{
    if (!ItemClass) return nullptr;

    // 1. Line Trace를 통한 바닥 정밀 감지 (기존 로직 유지 및 최적화)
    FVector Start = FVector(InLocation.X, InLocation.Y, 500.0f); // 시작 높이를 조금 더 확보
    FVector End = FVector(InLocation.X, InLocation.Y, -500.0f);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 월드에서 MazeGenerator를 다시 찾지 않고, 필요한 경우만 무시 설정
    AActor* MazeGenActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMazeGenerator::StaticClass());
    if (MazeGenActor) Params.AddIgnoredActor(MazeGenActor);

    FVector FinalSpawnLocation = InLocation;

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        // ImpactPoint(충돌 지점)를 사용하여 정확한 바닥 안착
        FinalSpawnLocation = HitResult.ImpactPoint;
        FinalSpawnLocation.Z += 100.0f; // 아이템 절반 높이만큼 보정 (값은 아이템에 따라 조절)
    }

    return GetWorld()->SpawnActor<AActor>(ItemClass, FinalSpawnLocation, FRotator::ZeroRotator);
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
