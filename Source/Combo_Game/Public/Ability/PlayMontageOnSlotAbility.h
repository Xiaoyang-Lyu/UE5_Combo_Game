// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PlayMontageOnSlotAbility.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMontageFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMontageInterrupted);

/**
 * 在指定插槽播放蒙太奇动画的Ability基类
 */
UCLASS(Abstract, Blueprintable)
class COMBO_GAME_API UPlayMontageOnSlotAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UPlayMontageOnSlotAbility();

protected:
    // 要播放的蒙太奇动画
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
    UAnimMontage* MontageToPlay;

    // 播放动画的插槽名称
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
    FName SlotNodeName;

    // 动画播放速率
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (ClampMin = 0.1))
    float PlayRate;

    // 开始播放的位置
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (ClampMin = 0.0))
    float StartingPosition;

    // 蒙太奇完成时的委托
    UPROPERTY(BlueprintAssignable, Category = "Montage")
    FOnMontageFinished OnMontageFinished;

    // 蒙太奇被中断时的委托
    UPROPERTY(BlueprintAssignable, Category = "Montage")
    FOnMontageInterrupted OnMontageInterrupted;

public:
    // 主要的Ability激活函数
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // 结束Ability
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    // 可在蓝图中重写的事件
    UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "On Ability Activated")
    void K2_OnAbilityActivated();

    UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "On Montage Finished")
    void K2_OnMontageFinished();

    UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "On Montage Interrupted")
    void K2_OnMontageInterrupted();

    // 获取要播放的蒙太奇（可在蓝图中重写以实现动态选择）
    UFUNCTION(BlueprintNativeEvent, Category = "Montage")
    UAnimMontage* GetMontageToPlay() const;
    virtual UAnimMontage* GetMontageToPlay_Implementation() const;

    // 获取插槽名称（可在蓝图中重写）
    UFUNCTION(BlueprintNativeEvent, Category = "Montage")
    FName GetSlotName() const;
    virtual FName GetSlotName_Implementation() const;

private:
    // 蒙太奇结束回调
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 存储当前播放的蒙太奇实例ID
    int32 CurrentMontageInstanceID;

    // 存储AnimInstance的弱引用
    TWeakObjectPtr<UAnimInstance> AnimInstancePtr;
};