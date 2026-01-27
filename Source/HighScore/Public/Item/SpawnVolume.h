// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemSpawnRow.h"       // 우리가 정의한 구조체
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class HIGHSCORE_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
    ASpawnVolume();

    UFUNCTION(Category = "Spawning", BlueprintCallable)
    AActor* SpawnRandomItem();

    // 스폰 볼륨 내부에서 무작위 좌표를 얻어오는 함수
    UFUNCTION(Category = "Spawning", BlueprintCallable)
    FVector GetRandomPointInVolume() const;

    // 특정 아이템 클래스를 스폰하는 함수
    UFUNCTION(Category = "Spawning", BlueprintCallable)
    AActor* SpawnItem(TSubclassOf<AActor> ItemClass);

    FItemSpawnRow* GetRandomItem() const;

    UPROPERTY(Category = "Spawning", EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UDataTable> ItemDataTable;

private:
    UPROPERTY(Category = "Spawning", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> Scene;
    
    // 스폰 영역을 담당할 박스 컴포넌트
    UPROPERTY(Category = "Spawning", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBoxComponent> SpawningBox;


};
