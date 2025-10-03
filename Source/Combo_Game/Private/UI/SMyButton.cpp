// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SMyButton.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION



void SMyButton::Construct(const FArguments& InArgs)
{
    ButtonText = InArgs._ButtonText;
    OnClickedDelegate = InArgs._OnClicked;

    ChildSlot
    [
        SNew(SButton)
        .OnClicked(this, &SMyButton::HandleClick)
        .Content()
        [
            SNew(STextBlock)
            .Text(FText::FromString(ButtonText))
        ]
    ];
}

FReply SMyButton::HandleClick()
{
    UE_LOG(LogTemp, Warning, TEXT("SMyButton点击: %s"), *ButtonText);
    
    if (OnClickedDelegate.IsBound())
    {
        return OnClickedDelegate.Execute();
    }
    
    return FReply::Handled();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

