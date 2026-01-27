// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/MineItem.h"
#include "Components/SphereComponent.h"

AMineItem::AMineItem()
{
    ExplosionDelay = 5.0f;
    ExplosionRadius = 300.0f;
    ExplosionDamage = 30.0f;
    ItemType = "Mine";

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::Explode()
{
    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Player damaged %d by MineItem"), ExplosionDamage));
        }
    }

    // 지뢰 제거
    DestroyItem();
}

void AMineItem::ActivateItem(AActor* Activator)
{
    // 5초 후 폭발 실행
    GetWorld()->GetTimerManager().SetTimer(
        ExplosionTimerHandle, 
        this, 
        &AMineItem::Explode, 
        ExplosionDelay
    );
}
