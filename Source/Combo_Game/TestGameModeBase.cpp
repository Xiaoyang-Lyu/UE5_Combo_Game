// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGameModeBase.h"
void ATestGameModeBase::StartPlay()
{
    Super::StartPlay();

    // 创建UI管理器
    UIManager = NewObject<UUIManager>();
    
    // 延迟创建UI
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
    {
        if (UIManager)
        {
            UIManager->CreateMainUI();
        }
    }, 1.0f, false);
}
