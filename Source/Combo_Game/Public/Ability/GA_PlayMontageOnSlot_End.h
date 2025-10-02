// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_PlayMontageOnSlot_End.generated.h"

/**
 * 
 */
UCLASS()
class COMBO_GAME_API UGA_PlayMontageOnSlot_End : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_PlayMontageOnSlot_End();

	/** 要播放的蒙太奇 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> MontageToPlay = nullptr;

	/** 仅用于校验/提示：要求 Montage 包含该 Slot（不匹配时会 Log 警告，但仍可播放） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	FName TargetSlotName = TEXT("DefaultSlot");

	/** 打断同组正在播放的蒙太奇再播；为 false 时，若同组在播则不播放、直接结束 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	bool bInterruptIfSameGroupPlaying = true;

	/** 打断同组时的淡出时间 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (EditCondition = "bInterruptIfSameGroupPlaying", ClampMin = "0.0"))
	float InterruptBlendOutTime = 0.2f;

	/** 播放速率 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (ClampMin = "0.01"))
	float PlayRate = 1.f;

	/** 起始 Section（可留空） */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	FName StartSection = NAME_None;

	/** 激活：播放并立刻结束 Ability */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	/** 蒙太奇是否包含某个 Slot（仅用于校验/提示） */
	static bool MontageHasSlot(const UAnimMontage* Montage, FName SlotName);

	/** 获取蒙太奇所属 Group 名（用于同组并发控制） */
	static FName GetMontageGroupName(const UAnimMontage* Montage);
};
