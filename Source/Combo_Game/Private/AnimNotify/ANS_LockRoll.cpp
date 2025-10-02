// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ANS_LockRoll.h"


#include "Player/ComboASComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Engine/Engine.h"

UComboASComponent* UANS_LockRoll::FindComboAS(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp) return nullptr;
	if (AActor* Owner = MeshComp->GetOwner())
	{
		return Owner->FindComponentByClass<UComboASComponent>();
	}
	return nullptr;
}

void UANS_LockRoll::DoLock(USkeletalMeshComponent* MeshComp) const
{
	if (UComboASComponent* ASC = FindComboAS(MeshComp))
	{
		ASC->LockRoll(true);
		if (bDebugLog)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[UANS_LockRoll] Lock -> %s"), *GetNameSafe(MeshComp->GetOwner()));
		}
	}
}

void UANS_LockRoll::DoUnlock(USkeletalMeshComponent* MeshComp) const
{
	if (UComboASComponent* ASC = FindComboAS(MeshComp))
	{
		ASC->LockRoll(false);
		if (bDebugLog)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[UANS_LockRoll] Unlock -> %s"), *GetNameSafe(MeshComp->GetOwner()));
		}
	}
}

void UANS_LockRoll::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	DoLock(MeshComp);
}

void UANS_LockRoll::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	// 无论是否被打断，只要离开这个状态区间，引擎都会调用 End，这里统一解锁即可
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	DoUnlock(MeshComp);
}