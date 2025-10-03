// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

// 前向声明
template<typename T>
class SComboBox;
class STextBlock;
/**
 *
 */
class COMBO_GAME_API SMovementSelector : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMovementSelector)
        : _DefaultSelection(TEXT("选择移动方式"))
        , _Width(200.0f)
    {}
    
    SLATE_ARGUMENT(FString, DefaultSelection)
    SLATE_ARGUMENT(float, Width)
    
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // 获取当前选中的移动方式
    FString GetSelectedMovement() const;

    // 设置选项
    void SetMovementOptions(const TArray<FString>& NewOptions);

private:
    // 生成选项显示文本
    FText GetCurrentText() const;
    
    // 生成下拉项控件
    TSharedRef<SWidget> GenerateComboBoxItem(TSharedPtr<FString> StringItem);

private:
    TSharedPtr<SComboBox<TSharedPtr<FString>>> ComboBox;
    TSharedPtr<STextBlock> DisplayTextBlock;
    
    TArray<TSharedPtr<FString>> Options;
    TSharedPtr<FString> CurrentSelection;
    
    FString DefaultSelection;
    float ComboBoxWidth;
};


