// Copyright (c) 2026 Junhyeok Choi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class HIGHSCORE_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// 블루프린트의 버튼 이름과 정확히 일치해야 합니다. (StartButton, ExitButton)
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;

	// 위젯 초기화 시 실행 (NativeConstruct는 BeginPlay와 유사함)
	virtual void NativeConstruct() override;

	// 버튼 클릭 시 호출될 함수
	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnExitClicked();
};
