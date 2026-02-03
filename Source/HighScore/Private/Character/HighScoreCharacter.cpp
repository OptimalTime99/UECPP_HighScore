// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#include "Character/HighScoreCharacter.h"
#include "Character/HighScorePlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HighScoreGameState.h"
#include "Components/ProgressBar.h"
#include "GAS/HighScoreAttributeSet.h"
#include "HighScoreHUD.h"

AHighScoreCharacter::AHighScoreCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    bUseControllerRotationYaw = false;
    
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 200.0f;  
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 150.f);

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;
    CameraComp->SetRelativeRotation(FRotator(-25.0f, 0.0f, 0.0f));

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
    OverheadWidget->SetupAttachment(GetMesh());
    OverheadWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
    
    // 1. ASC 생성
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

    // 복제(Replication) 설정 (멀티플레이 고려 시 필수)
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    // 2. AttributeSet 생성 (ASC가 생성된 후 생성해야 함)
    AttributeSet = CreateDefaultSubobject<UHighScoreAttributeSet>(TEXT("AttributeSet"));
}

void AHighScoreCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // 서버에서 InitAbilityActorInfo 호출
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

UAbilitySystemComponent* AHighScoreCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AHighScoreCharacter::BindAttributeCallbacks()
{
    if (AttributeSet)
    {
        // AttributeSet의 OnDeath 델리게이트에 캐릭터의 OnDeath 함수 연결
        AttributeSet->OnDeath.AddLambda([this](AActor* Killer)
            {
                this->OnDeath();
            });
    }
}

int32 AHighScoreCharacter::GetHealth() const
{
    // AttributeSet이 유효하면 그 값을, 아니면 0을 반환
    return AttributeSet ? static_cast<int32>(AttributeSet->GetHealth()) : 0;
}

int32 AHighScoreCharacter::GetMaxHealth() const
{
    return AttributeSet ? static_cast<int32>(AttributeSet->GetMaxHealth()) : 0;
}

void AHighScoreCharacter::BeginPlay()
{
    Super::BeginPlay();
    UpdateOverheadHP();

    if (AbilitySystemComponent && AttributeSet)
    {
        // Health가 변할 때마다 호출될 콜백 등록
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute())
            .AddUObject(this, &AHighScoreCharacter::OnHealthChanged);

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute())
            .AddUObject(this, &AHighScoreCharacter::OnHealthChanged);
    }

    BindAttributeCallbacks();
}

void AHighScoreCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced InputComponent로 캐스팅
    if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // IA를 가져오기 위해 현재 소유 중인 Controller를 AHighScorePlayerController로 캐스팅
        if (AHighScorePlayerController* PC = Cast<AHighScorePlayerController>(GetController()))
        {
            if (PC->MoveAction)
            {
                // IA_Move 액션 키를 "키를 누르고 있는 동안" Move() 호출
                EI->BindAction(
                    PC->MoveAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AHighScoreCharacter::Move
                );
            }

            if (PC->JumpAction)
            {
                // IA_Jump 액션 키를 "키를 누르고 있는 동안" StartJump() 호출
                EI->BindAction(
                    PC->JumpAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AHighScoreCharacter::StartJump
                );

                // IA_Jump 액션 키에서 "손을 뗀 순간" StopJump() 호출
                EI->BindAction(
                    PC->JumpAction,
                    ETriggerEvent::Completed,
                    this,
                    &AHighScoreCharacter::StopJump
                );
            }

            if (PC->LookAction)
            {
                // IA_Look 액션 마우스가 "움직일 때" Look() 호출
                EI->BindAction(
                    PC->LookAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AHighScoreCharacter::Look
                );
            }

            if (PC->SprintAction)
            {
                // IA_Sprint 액션 키를 "누르고 있는 동안" StartSprint() 호출
                EI->BindAction(
                    PC->SprintAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AHighScoreCharacter::StartSprint
                );
                // IA_Sprint 액션 키에서 "손을 뗀 순간" StopSprint() 호출
                EI->BindAction(
                    PC->SprintAction,
                    ETriggerEvent::Completed,
                    this,
                    &AHighScoreCharacter::StopSprint
                );
            }
        }
    }
}

void AHighScoreCharacter::Move(const FInputActionValue& value)
{
    // 컨트롤러가 있어야 방향 계산이 가능
    if (!Controller) return;

    // Value는 Axis2D로 설정된 IA_Move의 입력값 (WASD)을 담고 있음
    // 예) (X=1, Y=0) → 전진 / (X=-1, Y=0) → 후진 / (X=0, Y=1) → 오른쪽 / (X=0, Y=-1) → 왼쪽
    FVector2D MoveInput = value.Get<FVector2D>();

    if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.InvertControl"))))
    {
        // 입력값(X, Y)에 -1을 곱해 전후좌우를 모두 뒤집습니다.
        MoveInput *= -1.0f;
    }

    // 컨트롤러의 회전값 중 Yaw(좌우 회전)만 가져옴
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    // 컨트롤러 기준 정면과 오른쪽 방향 계산
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        // 캐릭터가 바라보는 방향(정면)으로 X축 이동
        AddMovementInput(ForwardDirection, MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        // 캐릭터의 오른쪽 방향으로 Y축 이동
        AddMovementInput(RightDirection, MoveInput.Y);
    }
}

void AHighScoreCharacter::StartJump(const FInputActionValue& value)
{
    // Jump 함수는 Character가 기본 제공
    if (value.Get<bool>())
    {
        Jump();
    }
}

void AHighScoreCharacter::StopJump(const FInputActionValue& value)
{
    // StopJumping 함수도 Character가 기본 제공
    if (!value.Get<bool>())
    {
        StopJumping();
    }
}

void AHighScoreCharacter::Look(const FInputActionValue& value)
{
    // 마우스의 X, Y 움직임을 2D 축으로 가져옴
    FVector2D LookInput = value.Get<FVector2D>();

    // X는 좌우 회전 (Yaw), Y는 상하 회전 (Pitch)
    // 좌우 회전
    AddControllerYawInput(LookInput.X);
    // 상하 회전
    AddControllerPitchInput(LookInput.Y);
}

void AHighScoreCharacter::StartSprint(const FInputActionValue& value)
{
    if (!AbilitySystemComponent || !SprintEffectClass) return;

    // 1. 슬로우 상태인지 체크 (기존 로직 유지)
    if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Slow"))))
    {
        return;
    }

    // 2. 이미 스프린트 중이라면 중복 적용 방지
    if (SprintEffectHandle.IsValid()) return;

    // 3. GE_Sprint 적용
    FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
    ContextHandle.AddInstigator(this, this);

    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(SprintEffectClass, 1.0f, ContextHandle);
    if (SpecHandle.IsValid())
    {
        // Infinite 정책을 가진 GE를 적용하고 핸들을 저장합니다.
        SprintEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

void AHighScoreCharacter::StopSprint(const FInputActionValue& value)
{
    if (!AbilitySystemComponent || !SprintEffectHandle.IsValid()) return;

    // 4. 저장해둔 핸들을 사용하여 스프린트 GE 제거
    AbilitySystemComponent->RemoveActiveGameplayEffect(SprintEffectHandle);
    SprintEffectHandle.Invalidate(); // 핸들 초기화
}

// 사망 처리 함수
void AHighScoreCharacter::OnDeath()
{
    if (AHighScoreGameState * HighScoreGS = GetWorld()->GetGameState<AHighScoreGameState>())
    {
        HighScoreGS->OnGameOver();
    }
}

void AHighScoreCharacter::UpdateOverheadHP()
{
    if (!OverheadWidget || !AttributeSet) return;

    UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
    if (!OverheadWidgetInstance) return;

    if (UProgressBar* HPBar = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverheadHP"))))
    {
        // 이제 캐릭터의 변수가 아닌 AttributeSet의 값을 사용합니다.
        const float HPPercent = AttributeSet->GetMaxHealth() > 0 ? AttributeSet->GetHealth() / AttributeSet->GetMaxHealth() : 0.0f;
        HPBar->SetPercent(HPPercent);
    }
}

void AHighScoreCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    // 1. 머리 위 HP 바 갱신
    UpdateOverheadHP();

    // 2. 메인 HUD 갱신
    if (AHighScorePlayerController* PC = Cast<AHighScorePlayerController>(GetController()))
    {
        if (AHighScoreHUD* HUD = Cast<AHighScoreHUD>(PC->GetHUD()))
        {
            // 체력만 쏙 바꿉니다. 점수나 시간은 건드리지 않아요!
            HUD->UpdateHealth(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
        }
    }
}

