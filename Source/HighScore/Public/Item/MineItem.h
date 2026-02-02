// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "MineItem.generated.h"


UCLASS()
class HIGHSCORE_API AMineItem : public ABaseItem
{
    GENERATED_BODY()

public:
    AMineItem();

    void Explode();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void ActivateItem(AActor* Activator) override;

private:
    // 폭발까지 걸리는 시간
    UPROPERTY(Category = "Mine", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float ExplosionDelay;
    // 폭발 범위
    UPROPERTY(Category = "Mine", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float ExplosionRadius;
    // 폭발 데미지
    UPROPERTY(Category = "Mine", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    int ExplosionDamage;

    UPROPERTY(Category = "Item|Component", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> ExplosionCollision;

    // 지뢰 발동 여부
    bool bHasExploded;
    FTimerHandle ExplosionTimerHandle;


    UPROPERTY(Category = "Item|Effects", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UParticleSystem> ExplosionParticle;

    UPROPERTY(Category = "Item|Effects", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USoundBase> ExplosionSound;

    // 경고 장판 데칼 컴포넌트
    UPROPERTY(Category = "Item|Component", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UDecalComponent> WarningDecal;

    UPROPERTY()
    class UMaterialInstanceDynamic* DynamicWarningMat;

    // 폭발까지 남은 시간을 추적하기 위한 변수
    float ElapsedTime;

    // 매 프레임 비주얼을 업데이트할 함수
    void UpdateWarningVisual(float DeltaTime);
};
