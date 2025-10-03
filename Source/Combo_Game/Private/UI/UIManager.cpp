// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/UIManager.h"
#include "UI/SMyButton.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/GameViewportClient.h"

void UUIManager::CreateMainUI()
{
    // 检查是否已经存在窗口
    if (MainWindow.IsValid())
    {
        return;
    }

    // 创建主窗口
    MainWindow = SNew(SWindow)
        .Title(FText::FromString("Game UI"))
        .ClientSize(FVector2D(600, 400))
        .SupportsMaximize(true)
        .SupportsMinimize(true)
        .SizingRule(ESizingRule::UserSized)
        [
            SNew(SVerticalBox)
            
            // 标题区域
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(10.0f)
            [
                SNew(STextBlock)
                .Text(FText::FromString("Combo Menu"))
                .Justification(ETextJustify::Center)
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(10.0f)
            [
                SNew(SHorizontalBox)
                
                // 添加横向框按钮
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(5.0f)
                [
                    SNew(SButton)
                    .Text(FText::FromString("Add Combo Series"))
                    .OnClicked_Lambda([this]()
                    {
                        this->AddHorizontalBox();
                        return FReply::Handled();
                    })
                ]
            ]
            
            // 主容器 - 存放所有横向框
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            .Padding(10.0f)
            [
                SAssignNew(MainContainer, SVerticalBox)
            ]
        ];

    // 添加到游戏视口
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->AddViewportWidgetForPlayer(
            GEngine->GetFirstLocalPlayerController(GWorld)->GetLocalPlayer(),
            MainWindow.ToSharedRef(),
            true
        );
    }
}

void UUIManager::CloseUI()
{
    if (MainWindow.IsValid())
    {
        MainWindow->RequestDestroyWindow();
        MainWindow.Reset();
    }
}


void UUIManager::AddHorizontalBox()
{
    if (!MainContainer.IsValid()) return;
    
    int32 BoxIndex = HorizontalBoxContainers.Num();
    
    // 创建横向框容器
    TSharedPtr<SHorizontalBox> HorizontalBoxInHorizontal;
    TSharedPtr<SHorizontalBox> NewHorizontalBox;
    
    MainContainer->AddSlot()
    .AutoHeight()
    .Padding(0.0f, 5.0f)
    [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
        .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.2f, 0.8f))
        .Padding(10.0f)
        [
            SNew(SVerticalBox)
            
            // 横向框标题和添加按钮
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 0.0f, 0.0f, 5.0f)
            [
                SNew(SHorizontalBox)
                
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(0.0f, 0.0f, 10.0f, 0.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(FString::Printf(TEXT("Combo Series %d"), BoxIndex + 1)))
                    .ColorAndOpacity(FLinearColor::Yellow)
                ]
                
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SButton)
                    .Text(FText::FromString("Add Movement"))
                    .OnClicked_Lambda([this, BoxIndex]()
                    {
                        this->AddMovement(BoxIndex, FString::Printf(TEXT("按钮 %d"), FMath::RandRange(1, 100)));
                        return FReply::Handled();
                    })
                ]
                
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(10.0f, 0.0f, 0.0f, 0.0f)
                [
                    SNew(SButton)
                    .Text(FText::FromString("删除此框"))
                    .ButtonColorAndOpacity(FLinearColor::Red)
                    .OnClicked_Lambda([this, BoxIndex]()
                    {
                        this->RemoveHorizontalBox(BoxIndex);
                        return FReply::Handled();
                    })
                ]
            ]
            
            // 实际的横向布局 - 里面包含纵向按钮
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SAssignNew(NewHorizontalBox, SHorizontalBox)
            ]
            
            // 纵向按钮容器（在横向框内部）
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SAssignNew(HorizontalBoxInHorizontal, SHorizontalBox)
            ]
        ]
    ];
    
    // 保存引用
    HorizontalBoxContainers.Add(HorizontalBoxInHorizontal);
    
    UE_LOG(LogTemp, Warning, TEXT("添加了横向框 %d"), BoxIndex);
}

void UUIManager::AddMovement(int32 BoxIndex, const FString& ButtonText)
{
    if (!HorizontalBoxContainers.IsValidIndex(BoxIndex)) return;
    
    TSharedPtr<SHorizontalBox> TargetBox = HorizontalBoxContainers[BoxIndex];
    if (!TargetBox.IsValid()) return;
    
    // int32 CurrentButtonCount = HorizontalBoxContainers->GetChildren()->Num();
    // // 如果不是第一个按钮，先添加箭头
    // if (CurrentButtonCount > 0)
        TargetBox->AddSlot()
        .AutoWidth()
        .Padding(5.0f, 0.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString("-->"))
            .ColorAndOpacity(FLinearColor::Yellow)
            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
        ];
    TargetBox->AddSlot()
    .AutoWidth()
    .Padding(0.0f, 2.0f)
    [
            SNew(SButton)
            .Text(FText::FromString(ButtonText))
            .OnClicked_Lambda([ButtonText, BoxIndex]()
            {
                UE_LOG(LogTemp, Warning, TEXT("横向框 %d 的按钮被点击: %s"), BoxIndex + 1, *ButtonText);
                return FReply::Handled();
            })
            .Content()
            [
                SNew(STextBlock)
                .Text(FText::FromString(ButtonText))
                .Justification(ETextJustify::Center)
            ]

    ];
    
    UE_LOG(LogTemp, Warning, TEXT("向横向框 %d 添加按钮: %s"), BoxIndex + 1, *ButtonText);
}

void UUIManager::RemoveHorizontalBox(int32 BoxIndex)
{
    if (!MainContainer.IsValid() || !HorizontalBoxContainers.IsValidIndex(BoxIndex)) return;
    
    // 从主容器中移除（这里需要更复杂的逻辑来精确移除）
    // 简化版：清空并重新添加其他框
    MainContainer->ClearChildren();
    HorizontalBoxContainers.RemoveAt(BoxIndex);
    
    // 重新添加剩余的框
    for (int32 i = 0; i < HorizontalBoxContainers.Num(); i++)
    {
        // 这里需要重新创建UI，简化示例
        UE_LOG(LogTemp, Warning, TEXT("需要重新实现删除逻辑"));
    }
}
