// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_PlayMontageOnSlot_End.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"

UGA_PlayMontageOnSlot_End::UGA_PlayMontageOnSlot_End()
{
	// 本地预测：允许客户端发起播放并由 ASC 进行复制
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 通常此类能力无需每次实例化多个
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_PlayMontageOnSlot_End::MontageHasSlot(const UAnimMontage* Montage, FName SlotName)
{
	if (!Montage || SlotName.IsNone()) return true; // 不做严格校验
	for (const FSlotAnimationTrack& Track : Montage->SlotAnimTracks)
	{
		if (Track.SlotName == SlotName)
		{
			return true;
		}
	}
	return false;
}

FName UGA_PlayMontageOnSlot_End::GetMontageGroupName(const UAnimMontage* Montage)
{
	// UE5 中 UAnimMontage 有 GroupName（由 Slot 决定），不同版本 API 名称略有差异；
	// 如无公开 Getter，也可从第一个 SlotTrack 推断组，或在打断时直接使用 StopGroupByName(组名)。
	// 这里优先使用 Montage 自带的 GroupName（若无，则返回 NAME_None）
#if ENGINE_MAJOR_VERSION >= 5
	// 新版通常提供 GetGroupName。若你的引擎没有此接口，可改为：
	// return Montage && Montage->SlotAnimTracks.Num() > 0 ? Montage->SlotAnimTracks[0].SlotName : NAME_None;
	return Montage ? Montage->GetGroupName() : NAME_None;
#else
	return Montage ? Montage->GetGroupName() : NAME_None;
#endif
}

void UGA_PlayMontageOnSlot_End::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 基础校验
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEndAbility=*/false, /*bWasCancelled=*/true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

	if (!MontageToPlay || !AnimInstance)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// 仅校验：蒙太奇是否包含目标 Slot（不匹配只警告，不阻止播放）
	if (!TargetSlotName.IsNone() && !MontageHasSlot(MontageToPlay, TargetSlotName))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[GA_PlayMontageInstant] Montage %s does not contain Slot %s"),
			*GetNameSafe(MontageToPlay), *TargetSlotName.ToString());
	}

	// 组并发处理：若同组在播，按配置决定是否打断
	const FName GroupName = GetMontageGroupName(MontageToPlay);

	if (!bInterruptIfSameGroupPlaying)
	{
		// 粗略判定：如果当前活动蒙太奇与目标组相同，则不播放
		if (UAnimMontage* Active = AnimInstance->GetCurrentActiveMontage())
		{
			// 有些版本没有 GetGroupName()，如遇到编译问题，可直接 End（保守）
#if ENGINE_MAJOR_VERSION >= 5
			const bool bSameGroup = (Active->GetGroupName() == GroupName) || GroupName.IsNone();
#else
			const bool bSameGroup = (Active->GetGroupName() == GroupName) || GroupName.IsNone();
#endif
			if (bSameGroup)
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, false, /*bWasCancelled=*/true);
				return;
			}
		}
	}
	else
	{
		// 打断同组
		if (!GroupName.IsNone())
		{
			AnimInstance->Montage_StopGroupByName(InterruptBlendOutTime, GroupName);
		}
		else
		{
			// 无法取得组名时，回退为停止当前活动蒙太奇（避免并发冲突）
			if (UAnimMontage* Active = AnimInstance->GetCurrentActiveMontage())
			{
				AnimInstance->Montage_Stop(InterruptBlendOutTime, Active);
			}
		}
	}

	// Commit 放在真正要播之前，避免无意义消耗
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// 播放（ASC 负责网络复制与预测）
	{
		FScopedPredictionWindow PredictWindow(ASC);
		const float Len = ASC->PlayMontage(this, ActivationInfo, MontageToPlay, PlayRate, StartSection, /*StartTime=*/0.f);
		if (Len <= 0.f)
		{
			// 播放失败，回退
			EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
			return;
		}
	}

	// Fire-and-Forget：立即结束 Ability（蒙太奇继续播放）
	EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEndAbility=*/false, /*bWasCancelled=*/false);
}