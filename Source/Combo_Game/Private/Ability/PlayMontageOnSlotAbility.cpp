#include "Ability/PlayMontageOnSlotAbility.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UPlayMontageOnSlotAbility::UPlayMontageOnSlotAbility()
{
    // 默认设置
    SlotNodeName = FName("DefaultSlot");
    PlayRate = 1.0f;
    StartingPosition = 0.0f;
    CurrentMontageInstanceID = INDEX_NONE;

    // 设置默认的实例化策略
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPlayMontageOnSlotAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 获取Character
    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 获取AnimInstance
    UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInstance)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 获取要播放的蒙太奇
    UAnimMontage* Montage = GetMontageToPlay();
    if (!Montage)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayMontageOnSlotAbility: No montage to play"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 获取插槽名称
    FName SlotName = GetSlotName();

    // 检查蒙太奇是否包含指定的插槽
    if (!Montage->IsValidSlot(SlotName))
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayMontageOnSlotAbility: Montage does not contain slot %s"), *SlotName.ToString());
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 提交Ability
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 保存AnimInstance的弱引用
    AnimInstancePtr = AnimInstance;

    // 绑定蒙太奇结束委托
    FOnMontageEnded MontageEndedDelegate;
    MontageEndedDelegate.BindUObject(this, &UPlayMontageOnSlotAbility::OnMontageEnded);

    // 播放蒙太奇
    CurrentMontageInstanceID = AnimInstance->Montage_PlayWithBlendIn(
        Montage,
        FAlphaBlendArgs(),
        PlayRate,
        EMontagePlayReturnType::MontageLength,
        StartingPosition,
        true
    );

    if (CurrentMontageInstanceID != INDEX_NONE)
    {
        // 设置蒙太奇结束委托
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);

        // 调用蓝图事件
        K2_OnAbilityActivated();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayMontageOnSlotAbility: Failed to play montage"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void UPlayMontageOnSlotAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    /*
    // 如果动画还在播放，停止它
    if (AnimInstancePtr.IsValid() && CurrentMontageInstanceID != INDEX_NONE)
    {
        if (UAnimMontage* cCurrentMontage = AnimInstancePtr->GetCurrentActiveMontage())
        {
            AnimInstancePtr->Montage_Stop(0.25f, cCurrentMontage);
        }
    }

    // 清理
    CurrentMontageInstanceID = INDEX_NONE;
    AnimInstancePtr.Reset();*/

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPlayMontageOnSlotAbility::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (bInterrupted)
    {
        OnMontageInterrupted.Broadcast();
        K2_OnMontageInterrupted();
    }
    else
    {
        OnMontageFinished.Broadcast();
        K2_OnMontageFinished();
    }

    // 结束Ability
    if (IsActive())
    {
        bool bReplicateEndAbility = true;
        bool bWasCancelled = bInterrupted;
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
    }
}

UAnimMontage* UPlayMontageOnSlotAbility::GetMontageToPlay_Implementation() const
{
    return MontageToPlay;
}

FName UPlayMontageOnSlotAbility::GetSlotName_Implementation() const
{
    return SlotNodeName;
}