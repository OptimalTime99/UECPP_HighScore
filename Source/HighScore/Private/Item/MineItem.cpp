// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h" // DestroyComponent()
#include "HighScoreGameState.h"
#include "Components/DecalComponent.h"

AMineItem::AMineItem()
{
    PrimaryActorTick.bCanEverTick = true;

    ExplosionDelay = 1.0f;
    ExplosionRadius = 300.0f;
    ExplosionDamage = 30;
    ItemType = "Mine";
    bHasExploded = false;
    ElapsedTime = 0.0f;

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);

    // 데칼 컴포넌트 생성 및 설정
    WarningDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("WarningDecal"));
    WarningDecal->SetupAttachment(RootComponent);


}

void AMineItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    if (bHasExploded)
    {
        // 경고 장판 비주얼 업데이트 호출
        UpdateWarningVisual(DeltaTime);
    }

}

void AMineItem::BeginPlay()
{
    Super::BeginPlay();

    if (AHighScoreGameState* GS = GetWorld()->GetGameState<AHighScoreGameState>())
    {
        int32 Level = GS->GetCurrentLevelIndex();
        ExplosionDelay = FMath::Min(ExplosionDelay, ExplosionDelay - (Level * 0.3f));

        int32 Wave = GS->GetCurrentWaveIndex();
        ExplosionDamage = FMath::Max(ExplosionDamage, ExplosionDamage + ((Wave - 1) * 5));
        ExplosionRadius = FMath::Max(ExplosionRadius, ExplosionRadius + ((Wave - 1) * 100));
        ExplosionCollision->SetSphereRadius(ExplosionRadius);
    }

    if (WarningDecal)
    {
        // 2. 기본 회전 및 위치 (바닥을 향하도록)
        // 데칼은 기본적으로 X축 방향으로 쏘기 때문에 -90도 회전이 필수입니다.
        WarningDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
        WarningDecal->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f)); // 지뢰보다 살짝 위에서 아래로 쏨

        // 3. 투사 크기 설정 (중요!)
        // X: 투사 깊이(길이), Y/Z: 장판의 반지름
        // X값이 너무 작으면 바닥에 닿기 전에 레이저가 끊깁니다.
        WarningDecal->DecalSize = FVector(100.0f, ExplosionRadius, ExplosionRadius);
        WarningDecal->SetVisibility(false);

        DynamicWarningMat = WarningDecal->CreateDynamicMaterialInstance();
    }
}

void AMineItem::UpdateWarningVisual(float DeltaTime)
{
    // 1. 다이내믹 머티리얼과 폭발 지연 시간이 유효한지 확인
    if (DynamicWarningMat && ExplosionDelay > 0.0f)
    {
        // 2. 경과 시간 누적
        ElapsedTime += DeltaTime;

        // 3. 진행률 계산 (0.0 ~ 1.0)
        float ProgressValue = FMath::Clamp(ElapsedTime / ExplosionDelay, 0.0f, 1.0f);

        // 4. 머티리얼의 'Progress' 파라미터 값 설정
        // 이 값이 머티리얼의 Step 노드와 연동되어 원을 키웁니다.
        DynamicWarningMat->SetScalarParameterValue(TEXT("Progress"), ProgressValue);
    }
}

void AMineItem::Explode()
{
    UParticleSystemComponent* Particle = nullptr;

    // 장판 제거
    if (WarningDecal)
    {
        WarningDecal->DestroyComponent();
    }

    if (ExplosionParticle)
    {
        Particle = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionParticle,
            GetActorLocation(),
            GetActorRotation(),
            false
        );
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ExplosionSound,
            GetActorLocation()
        );
    }

    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            // 데미지를 발생시켜 Actor->TakeDamage()가 실행되도록 함
            UGameplayStatics::ApplyDamage(
                Actor,                      // 데미지를 받을 액터
                ExplosionDamage,            // 데미지 양
                nullptr,                    // 데미지를 유발한 주체 (지뢰를 설치한 캐릭터가 없으므로 nullptr)
                this,                       // 데미지를 유발한 오브젝트(지뢰)
                UDamageType::StaticClass()  // 기본 데미지 유형
            );
        }
    }

    // 폭발 이후 지뢰 아이템 파괴
    DestroyItem();

    if (Particle)
    {
        FTimerHandle DestroyParticleTimerHandle;
        TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

        GetWorld()->GetTimerManager().SetTimer(
            DestroyParticleTimerHandle,
            [WeakParticle]()
            {
                if (WeakParticle.IsValid())
                {
                    WeakParticle->DestroyComponent();
                }
            },
            2.0f,
            false
        );
    }
}

void AMineItem::ActivateItem(AActor* Activator)
{
    if (bHasExploded) return;
    Super::ActivateItem(Activator);

    WarningDecal->SetVisibility(true);

    // ExplosionDelay초 후 폭발 실행
    GetWorld()->GetTimerManager().SetTimer(
        ExplosionTimerHandle,
        this,
        &AMineItem::Explode,
        ExplosionDelay
    );

    bHasExploded = true;
}
