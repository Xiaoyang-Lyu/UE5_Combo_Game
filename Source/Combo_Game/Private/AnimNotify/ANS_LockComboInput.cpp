// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ANS_LockComboInput.h"

#include "Player/ComboASComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Engine/Engine.h"

UComboASComponent* UANS_LockComboInput::FindComboAS(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp) return nullptr;
	if (AActor* Owner = MeshComp->GetOwner())
	{
		return Owner->FindComponentByClass<UComboASComponent>();
	}
	return nullptr;
}

void UANS_LockComboInput::DoLock(USkeletalMeshComponent* MeshComp) const
{
	if (UComboASComponent* ASC = FindComboAS(MeshComp))
	{
		ASC->LockComboInput(true);
		if (bDebugLog)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[UANS_LockComboInput] Lock -> %s"), *GetNameSafe(MeshComp->GetOwner()));
		}
	}
}

void UANS_LockComboInput::DoUnlock(USkeletalMeshComponent* MeshComp) const
{
	if (UComboASComponent* ASC = FindComboAS(MeshComp))
	{
		ASC->LockComboInput(false);
		if (bDebugLog)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[UANS_LockComboInput] Unlock -> %s"), *GetNameSafe(MeshComp->GetOwner()));
		}
	}
}

void UANS_LockComboInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	DoLock(MeshComp);
}

void UANS_LockComboInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	// �����Ƿ񱻴�ϣ�ֻҪ�뿪���״̬���䣬���涼����� End������ͳһ��������
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	DoUnlock(MeshComp);
}
