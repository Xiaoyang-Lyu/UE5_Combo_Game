// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/ComboASComponent.h"
#include "UObject/Object.h"
#include "Widgets/SWidget.h"

#include "UIManager.generated.h"

// 前向声明Slate类型
class SWindow;
class SMyButton;
class UComboASDataAsset;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
    UComboASDataAsset* ComboDataAsset;

  private:
    // 保存窗口引用
    TSharedPtr<SWindow> MainWindow;

    TArray<TSharedPtr<SHorizontalBox>> HorizontalBoxContainers;
    TSharedPtr<SVerticalBox>         MainContainer;

    // 方法声明
    void AddHorizontalBox();
    void AddMovement(int32 BoxIndex, const FString& ButtonText);

void RemoveHorizontalBox(int32 BoxIndex);

void AddPatternLabel(int32 GroupIndex, TSharedPtr<SVerticalBox> Container);

FString GetArrowPattern(int32 GroupIndex) const;
};
