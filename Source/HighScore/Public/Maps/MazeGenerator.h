// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeGenerator.generated.h"

// 각 셀의 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FMazeCell
{
    GENERATED_BODY()

    int32 X = 0;
    int32 Y = 0;
    bool bVisited = false;
    bool bHasRightWall = true;
    bool bHasBottomWall = true;
};

UCLASS()
class HIGHSCORE_API AMazeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AMazeGenerator();

    // 길(Cell)의 중심 좌표들을 반환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Maze")
    TArray<FVector> GetMazePathLocations() const;

protected:
	virtual void BeginPlay() override;

    // 미로 설정값
    UPROPERTY(EditAnywhere, Category = "Maze Settings")
    int32 MapWidth = 12;

    UPROPERTY(EditAnywhere, Category = "Maze Settings")
    int32 MapHeight = 12;

    UPROPERTY(EditAnywhere, Category = "Maze Settings")
    float TileSize = 500.0f;

    // 에디터에서 Wall_500x500 액터를 선택하세요
    UPROPERTY(EditAnywhere, Category = "Maze Settings")
    TSubclassOf<AActor> WallClass;

private:
    TArray<TArray<FMazeCell>> Grid;

    void InitializeGrid();
    void GenerateMaze(int32 StartX, int32 StartY);
    void SpawnMazeWalls();

};
