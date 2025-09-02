// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_LockRoll.generated.h"

class UComboASComponent;

/**
 * 
 */
UCLASS()
class COMBO_GAME_API UANS_LockRoll : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	/** 仅用于开发期日志 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockCombo")
	bool bDebugLog = false;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;


private:
	static UComboASComponent* FindComboAS(USkeletalMeshComponent* MeshComp);
	void DoLock(USkeletalMeshComponent* MeshComp) const;
	void DoUnlock(USkeletalMeshComponent* MeshComp) const;
};
