// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "HighScoreCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AHighScoreCharacter::AHighScoreCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// (1) 스프링 암 생성
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// 스프링 암을 루트 컴포넌트 (CapsuleComponent)에 부착
	SpringArmComp->SetupAttachment(RootComponent);
	// 캐릭터와 카메라 사이의 거리 기본값 200으로 설정
	SpringArmComp->TargetArmLength = 200.0f;  
	// 컨트롤러 회전에 따라 스프링 암도 회전하도록 설정
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 150.f);

	// (2) 카메라 컴포넌트 생성
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// 스프링 암의 소켓 위치에 카메라를 부착
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	// 카메라는 스프링 암의 회전을 따르므로 PawnControlRotation은 꺼둠
	CameraComp->bUsePawnControlRotation = false;
	CameraComp->SetRelativeRotation(FRotator(-25.0f, 0.0f, 0.0f));

}

void AHighScoreCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHighScoreCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

