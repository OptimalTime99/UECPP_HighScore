// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Maps/MazeGenerator.h"
#include "DrawDebugHelpers.h"

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

    // 초기 시작점 설정
    Grid[0][0].bVisited = true;
    GenerationStack.Push(&Grid[0][0]);

    // 0.3초마다 ExecuteGenerationStep 함수를 반복 호출 (속도 조절 가능)
    GetWorldTimerManager().SetTimer(GenerationTimerHandle, this, &AMazeGenerator::ExecuteGenerationStep, 0.3f, true);
	
}

void AMazeGenerator::ExecuteGenerationStep()
{
    // 1. 종료 조건: 스택이 비어있으면 탐색 완료
    if (GenerationStack.Num() <= 0)
    {
        GetWorldTimerManager().ClearTimer(GenerationTimerHandle);
        SpawnMazeWalls(); // 모든 경로 탐색 후 실제 벽 생성
        return;
    }

    // 2. 현재 작업 중인 셀 가져오기 (스택의 최상단)
    FMazeCell* Current = GenerationStack.Last();
    TArray<int32> UnvisitedNeighbors;

    // 방향 배열 (우, 좌, 하, 상)
    int32 DX[] = { 1, -1, 0, 0 };
    int32 DY[] = { 0, 0, 1, -1 };

    // 3. 인접한 방문하지 않은 이웃 찾기
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
        // 4. 무작위 방향 선택 및 이동
        int32 ChosenDir = UnvisitedNeighbors[FMath::RandRange(0, UnvisitedNeighbors.Num() - 1)];
        int32 NextX = Current->X + DX[ChosenDir];
        int32 NextY = Current->Y + DY[ChosenDir];

        // 5. 벽 제거 로직 (기존 로직 그대로 사용)
        if (ChosenDir == 0) Current->bHasRightWall = false;
        else if (ChosenDir == 2) Current->bHasBottomWall = false;
        else if (ChosenDir == 1) Grid[NextX][NextY].bHasRightWall = false;
        else if (ChosenDir == 3) Grid[NextX][NextY].bHasBottomWall = false;

        // 6. 시각화 (DrawDebugLine)
        float HalfWidth = (MapWidth * TileSize) * 0.5f;
        float HalfHeight = (MapHeight * TileSize) * 0.5f;
        FVector StartOffset = FVector(-HalfWidth, -HalfHeight, 0.0f);

        FVector StartPos = StartOffset + FVector(Current->X * TileSize + (TileSize * 0.5f), Current->Y * TileSize + (TileSize * 0.5f), 100.0f);
        FVector EndPos = StartOffset + FVector(NextX * TileSize + (TileSize * 0.5f), NextY * TileSize + (TileSize * 0.5f), 100.0f);

        DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Red, false, 5.0f, 0, 10.0f);

        // 7. 다음 셀 방문 처리 및 스택에 추가
        Grid[NextX][NextY].bVisited = true;
        GenerationStack.Push(&Grid[NextX][NextY]);
    }
    else
    {
        // 8. 이웃이 없으면 역추적 (Pop)
        GenerationStack.Pop();
    }
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

