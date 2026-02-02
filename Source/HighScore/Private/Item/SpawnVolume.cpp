// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Maps/MazeGenerator.h"
#include "Kismet/GameplayStatics.h"

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

    // 1. 월드에서 MazeGenerator 찾기
    AActor* MazeGenActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMazeGenerator::StaticClass());
    AMazeGenerator* MazeGen = Cast<AMazeGenerator>(MazeGenActor);

    FVector TargetLocation;

    if (MazeGen)
    {
        // 2. 미로의 길(Cell Center) 목록 중 랜덤 선택
        TArray<FVector> Locations = MazeGen->GetMazePathLocations();
        if (Locations.Num() > 0)
        {
            TargetLocation = Locations[FMath::RandRange(0, Locations.Num() - 1)];
        }
    }
    else
    {
        // 미로 생성기가 없을 경우 대비한 기본 랜덤 위치
        TargetLocation = GetRandomPointInVolume();
    }

    // 3. Line Trace로 바닥 높이 정밀 조정
    // 시작점: 선택된 좌표의 Z를 100으로 설정
    FVector Start = FVector(TargetLocation.X, TargetLocation.Y, 100.0f);
    // 끝점: 바닥 아래 충분히 낮은 곳 (-500)
    FVector End = FVector(TargetLocation.X, TargetLocation.Y, -500.0f);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this); // 스폰 볼륨 무시
    if (MazeGen) Params.AddIgnoredActor(MazeGen); // 미로 생성기 무시

    FVector FinalSpawnLocation = Start; // 기본값

    // 레이저 발사! (ECC_Visibility 채널 사용)
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        // 지면에 닿은 위치 사용
        FinalSpawnLocation = HitResult.Location;

        // [중요] 아이템 절반이 땅에 묻히지 않게 살짝 띄움 (Offset)
        FinalSpawnLocation.Z += 100.0f;
    }

    // 4. 최종 위치에 스폰
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
