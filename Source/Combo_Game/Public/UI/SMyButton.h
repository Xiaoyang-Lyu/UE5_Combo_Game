// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class COMBO_GAME_API SMyButton : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMyButton)
        : _ButtonText(TEXT("Button"))
    {}
    
    SLATE_ARGUMENT(FString, ButtonText)
    SLATE_EVENT(FOnClicked, OnClicked)
    
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply HandleClick();
    
    FString ButtonText;
    FOnClicked OnClickedDelegate;
};

