// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AN_ResetCombo.h"
#include "Player/ComboASComponent.h"

void UAN_ResetCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (MeshComp)
	{
		if (UComboASComponent* ASComp = MeshComp->GetOwner()->GetComponentByClass<UComboASComponent>())
		{
			ASComp->ResetCombo(DelayTime);
		}
	}
}
