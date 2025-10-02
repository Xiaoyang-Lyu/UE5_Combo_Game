#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_LockComboInput.generated.h"

class UComboASComponent;

/**
 * 在通知区间内锁定连击输入；无论蒙太奇自然结束还是被打断，都会在离开区间时解锁。
 */
UCLASS(meta = (DisplayName = "Lock Combo Input"))
class COMBO_GAME_API UANS_LockComboInput : public UAnimNotifyState
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
