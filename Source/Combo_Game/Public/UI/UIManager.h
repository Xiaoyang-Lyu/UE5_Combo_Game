// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Widgets/SWidget.h"

#include "UIManager.generated.h"

// 前向声明Slate类型
class SWindow;
class SMyButton;

/**
 * 专门负责创建和管理UI的类
 */
UCLASS()
class COMBO_GAME_API UUIManager : public UObject
{
    GENERATED_BODY()

  public:
    // 创建主UI窗口
    void CreateMainUI();

    // 关闭UI
    void CloseUI();

  private:
    // 保存窗口引用
    TSharedPtr<SWindow> MainWindow;

    TArray<TSharedPtr<SVerticalBox>> HorizontalBoxContainers;
    TSharedPtr<SVerticalBox>         MainContainer;

    // 方法声明
    void AddHorizontalBox();
    void AddVerticalButtonToBox(int32 BoxIndex, const FString& ButtonText);

void RemoveHorizontalBox(int32 BoxIndex);
};
