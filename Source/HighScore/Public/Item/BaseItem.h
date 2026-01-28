// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemInterface.h"
#include "BaseItem.generated.h"

class USphereComponent;

UCLASS()
class HIGHSCORE_API ABaseItem : public AActor, public IItemInterface
{
    GENERATED_BODY()
    
public:	
    ABaseItem();

protected:
    // 아이템 유형(타입)을 편집 가능하게 지정
    UPROPERTY(Category = "Item", EditAnywhere, BlueprintReadWrite)
    FName ItemType;

    virtual void BeginPlay() override;

    // IItemInterface에서 요구하는 함수들을 반드시 구현
    UFUNCTION()
    virtual void OnItemOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult) override;

    UFUNCTION()
    virtual void OnItemEndOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex) override;

    virtual void ActivateItem(AActor* Activator) override;

    virtual FName GetItemType() const override;

    // 아이템을 제거하는 공통 함수 (추가 이펙트나 로직을 넣을 수 있음)
    virtual void DestroyItem();

    // 루트 컴포넌트 (씬)
    UPROPERTY(Category = "Item|Component", VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> Scene;

private:
    // 충돌 컴포넌트 (플레이어 진입 범위 감지용)
    UPROPERTY(Category = "Item|Component", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> Collision;
    // 아이템 시각 표현용 스태틱 메시
    UPROPERTY(Category = "Item|Component", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    UPROPERTY(Category = "Item|Effects", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UParticleSystem> PickupParticle;

    UPROPERTY(Category = "Item|Effects", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> PickupSound;



};
