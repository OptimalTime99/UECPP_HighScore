// Copyright (c) 2026 Junhyeok Choi. All rights reserved.


#include "Item/CoinItem.h"

ACoinItem::ACoinItem()
{
    // 부모 생성자 로직 (필요 시)
}

void ACoinItem::ActivateItem(AActor* Activator)
{
    // 플레이어 태그 확인
    if (Activator && Activator->ActorHasTag("Player"))
    {
        // 점수 획득 디버그 메시지
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player gained %d points!"), PointValue));

        // 부모 클래스 (BaseItem)에 정의된 아이템 파괴 함수 호출
        DestroyItem();
    }
}
