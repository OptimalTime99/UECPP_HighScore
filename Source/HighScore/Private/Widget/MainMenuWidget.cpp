// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Widget/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Character/HighScorePlayerController.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 클릭 이벤트에 함수 바인딩
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitClicked);
	}
}

void UMainMenuWidget::OnStartClicked()
{
	// 1. 현재 플레이어 컨트롤러를 가져와서 해당 클래스로 캐스팅
	AHighScorePlayerController* PC = Cast<AHighScorePlayerController>(GetOwningPlayer());

	if (PC)
	{
		// 2. 이미 구현되어 있는 StartGame 함수를 바로 호출
		PC->StartGame();
	}
}

void UMainMenuWidget::OnExitClicked()
{
	AHighScorePlayerController* PC = Cast<AHighScorePlayerController>(GetOwningPlayer());

	if (PC)
	{
		// 3. 이미 구현되어 있는 ExitGame 함수를 바로 호출
		PC->ExitGame();
	}
}

