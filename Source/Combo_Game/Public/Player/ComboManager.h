// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/UIManager.h"
#include "UObject/ObjectMacros.h"
#include "ComboManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBO_GAME_API UComboManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComboManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

 // 切换UI显示/隐藏
    UFUNCTION(BlueprintCallable, Category = "Combo UI")
    void ToggleUI();

    // 显示UI
    UFUNCTION(BlueprintCallable, Category = "Combo UI")
    void ShowUI();

    // 隐藏UI
    UFUNCTION(BlueprintCallable, Category = "Combo UI")
    void HideUI();

    UFUNCTION(BlueprintCallable, Category = "Combo UI")
    void SetupPlayerInputComponent();

private:
    // 按键绑定函数
    void OnToggleUIKeyPressed();

    // 输入模式切换
    void SwitchToUIMode();
    void SwitchToGameMode();

private:
    UPROPERTY()
    UUIManager* UIManager;

    UPROPERTY()
    class UInputComponent* InputComponent;

    bool bIsUIVisible;
    

		
};
