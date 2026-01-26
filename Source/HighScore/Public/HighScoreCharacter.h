// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HighScoreCharacter.generated.h"

class USpringArmComponent; // 스프링 암 관련 클래스 헤더
class UCameraComponent; // 카메라 관련 클래스 전방 선언

UCLASS()
class HIGHSCORE_API AHighScoreCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHighScoreCharacter();

protected:
	// 스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	// 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;public:	


};
