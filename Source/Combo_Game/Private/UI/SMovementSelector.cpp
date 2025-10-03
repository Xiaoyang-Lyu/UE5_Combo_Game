// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SMovementSelector.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "SlateOptMacros.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMovementSelector::Construct(const FArguments& InArgs)
{
    DefaultSelection = InArgs._DefaultSelection;
    ComboBoxWidth = InArgs._Width;

    // 初始化默认选项
    CurrentSelection = MakeShared<FString>(DefaultSelection);
    
    // 设置默认的移动方式选项
    TArray<FString> DefaultOptions = {
        "行走",
        "奔跑", 
        "潜行",
        "游泳",
        "飞行",
        "传送"
    };
    SetMovementOptions(DefaultOptions);

    // 构建控件
    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(ComboBoxWidth)
        [
            SAssignNew(ComboBox, SComboBox<TSharedPtr<FString>>)
            .OptionsSource(&Options)
            .OnSelectionChanged_Lambda([this](TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
            {
                // Lambda 处理选择变化
                if (NewSelection.IsValid())
                {
                    CurrentSelection = NewSelection;
                    
                    FString SelectedText = *CurrentSelection;
                    UE_LOG(LogTemp, Warning, TEXT("移动方式选择: %s"), *SelectedText);
                    
                    // 这里可以添加更多的选择处理逻辑
                    if (SelectedText == "行走")
                    {
                        UE_LOG(LogTemp, Log, TEXT("切换到行走模式"));
                    }
                    else if (SelectedText == "奔跑")
                    {
                        UE_LOG(LogTemp, Log, TEXT("切换到奔跑模式"));
                    }
                    else if (SelectedText == "飞行")
                    {
                        UE_LOG(LogTemp, Log, TEXT("切换到飞行模式"));
                    }
                }
            })
            .OnGenerateWidget(this, &SMovementSelector::GenerateComboBoxItem)
            .InitiallySelectedItem(CurrentSelection)
            .Content()
            [
                SAssignNew(DisplayTextBlock, STextBlock)
                .Text(this, &SMovementSelector::GetCurrentText)
                .Justification(ETextJustify::Center)
            ]
        ]
    ];
}

FText SMovementSelector::GetCurrentText() const
{
    if (CurrentSelection.IsValid())
    {
        return FText::FromString(*CurrentSelection);
    }
    return FText::FromString(DefaultSelection);
}

TSharedRef<SWidget> SMovementSelector::GenerateComboBoxItem(TSharedPtr<FString> StringItem)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*StringItem))
        .Margin(FMargin(4.0f));
}

FString SMovementSelector::GetSelectedMovement() const
{
    if (CurrentSelection.IsValid())
    {
        return *CurrentSelection;
    }
    return FString();
}

void SMovementSelector::SetMovementOptions(const TArray<FString>& NewOptions)
{
    Options.Empty();
    
    // 添加新选项
    for (const FString& Option : NewOptions)
    {
        Options.Add(MakeShared<FString>(Option));
    }
    
    // 刷新下拉列表
    if (ComboBox.IsValid())
    {
        ComboBox->RefreshOptions();
        
        // 如果没有选中项，选择第一个
        if (!CurrentSelection.IsValid() && Options.Num() > 0)
        {
            CurrentSelection = Options[0];
            ComboBox->SetSelectedItem(CurrentSelection);
        }
    }
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION



