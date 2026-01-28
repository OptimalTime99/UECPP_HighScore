// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/CoinItem.h"
#include "HighScoreGameState.h"

ACoinItem::ACoinItem()
{
    PointValue = 0;
    ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator); 

    // 플레이어 태그 확인
    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (UWorld* World = GetWorld())
        {
            if (AHighScoreGameState* GameState = World->GetGameState<AHighScoreGameState>())
            {
                GameState->AddScore(PointValue);
                GameState->OnCoinCollected();
            }
        }

        // 부모 클래스 (BaseItem)에 정의된 아이템 파괴 함수 호출
        DestroyItem();
    }
}
