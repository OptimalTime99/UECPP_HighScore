// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Maps/MazeGenerator.h"


// Sets default values
AMazeGenerator::AMazeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

}

TArray<FVector> AMazeGenerator::GetMazePathLocations() const
{
    TArray<FVector> PathLocations;
    float HalfWidth = (MapWidth * TileSize) * 0.5f;
    float HalfHeight = (MapHeight * TileSize) * 0.5f;
    FVector StartOffset = FVector(-HalfWidth, -HalfHeight, 0.0f);

    for (int32 x = 0; x < MapWidth; x++)
    {
        for (int32 y = 0; y < MapHeight; y++)
        {
            FVector CellCenter = StartOffset + FVector(x * TileSize + (TileSize * 0.5f), y * TileSize + (TileSize * 0.5f), 50.0f);
            PathLocations.Add(CellCenter);
        }
    }
    return PathLocations;
}

void AMazeGenerator::BeginPlay()
{
	Super::BeginPlay();

	InitializeGrid();
	GenerateMaze(0, 0); // (0,0) 좌표에서 미로 생성 시작
	SpawnMazeWalls();
	
}

void AMazeGenerator::InitializeGrid()
{
    Grid.Empty();
    Grid.SetNum(MapWidth);
    for (int32 i = 0; i < MapWidth; i++)
    {
        Grid[i].SetNum(MapHeight);
        for (int32 j = 0; j < MapHeight; j++)
        {
            Grid[i][j].X = i;
            Grid[i][j].Y = j;
            Grid[i][j].bVisited = false;
            Grid[i][j].bHasRightWall = true;
            Grid[i][j].bHasBottomWall = true;
        }
    }
}

void AMazeGenerator::GenerateMaze(int32 StartX, int32 StartY)
{
    TArray<FMazeCell*> Stack;
    Grid[StartX][StartY].bVisited = true;
    Stack.Push(&Grid[StartX][StartY]);

    // 방향 배열 (우, 좌, 하, 상)
    int32 DX[] = { 1, -1, 0, 0 };
    int32 DY[] = { 0, 0, 1, -1 };

    while (Stack.Num() > 0)
    {
        FMazeCell* Current = Stack.Last();
        TArray<int32> UnvisitedNeighbors;

        for (int32 i = 0; i < 4; i++)
        {
            int32 NextX = Current->X + DX[i];
            int32 NextY = Current->Y + DY[i];

            if (NextX >= 0 && NextX < MapWidth && NextY >= 0 && NextY < MapHeight)
            {
                if (!Grid[NextX][NextY].bVisited)
                {
                    UnvisitedNeighbors.Add(i);
                }
            }
        }

        if (UnvisitedNeighbors.Num() > 0)
        {
            int32 ChosenDir = UnvisitedNeighbors[FMath::RandRange(0, UnvisitedNeighbors.Num() - 1)];
            int32 NextX = Current->X + DX[ChosenDir];
            int32 NextY = Current->Y + DY[ChosenDir];

            // 벽 제거 로직
            if (ChosenDir == 0) Current->bHasRightWall = false;      // 우측 이동: 현재 셀 우측벽 제거
            else if (ChosenDir == 2) Current->bHasBottomWall = false; // 하단 이동: 현재 셀 하단벽 제거
            else if (ChosenDir == 1) Grid[NextX][NextY].bHasRightWall = false;  // 좌측 이동: 다음 셀 우측벽 제거
            else if (ChosenDir == 3) Grid[NextX][NextY].bHasBottomWall = false; // 상단 이동: 다음 셀 하단벽 제거

            Grid[NextX][NextY].bVisited = true;
            Stack.Push(&Grid[NextX][NextY]);
        }
        else
        {
            Stack.Pop();
        }
    }
}

void AMazeGenerator::SpawnMazeWalls()
{
    if (!WallClass) return;

    float HalfWidth = (MapWidth * TileSize) * 0.5f;
    float HalfHeight = (MapHeight * TileSize) * 0.5f;
    FVector StartOffset = FVector(-HalfWidth, -HalfHeight, 0.0f);

    for (int32 x = 0; x < MapWidth; x++)
    {
        for (int32 y = 0; y < MapHeight; y++)
        {
            FVector CellTopLeft = StartOffset + FVector(x * TileSize, y * TileSize, 0.0f);

            // 1. 우측 벽 스폰 (셀의 오른쪽 변)
            if (Grid[x][y].bHasRightWall && x < MapWidth - 1)
            {
                FVector WallPos = CellTopLeft + FVector(TileSize, TileSize * 0.5f, 0.0f);
                GetWorld()->SpawnActor<AActor>(WallClass, WallPos, FRotator(0, 90, 0));
            }

            // 2. 하단 벽 스폰 (셀의 아래쪽 변)
            if (Grid[x][y].bHasBottomWall && y < MapHeight - 1)
            {
                FVector WallPos = CellTopLeft + FVector(TileSize * 0.5f, TileSize, 0.0f);
                GetWorld()->SpawnActor<AActor>(WallClass, WallPos, FRotator::ZeroRotator);
            }
        }
    }
}

