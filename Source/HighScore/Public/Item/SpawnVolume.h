// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class HIGHSCORE_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
    ASpawnVolume();

    // 스폰 볼륨 내부에서 무작위 좌표를 얻어오는 함수
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    FVector GetRandomPointInVolume() const;

    // 특정 아이템 클래스를 스폰하는 함수
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SpawnItem(TSubclassOf<AActor> ItemClass);

private:
    UPROPERTY(Category = "Spawning", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> Scene;
    
    // 스폰 영역을 담당할 박스 컴포넌트
    UPROPERTY(Category = "Spawning", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBoxComponent> SpawningBox;
};
