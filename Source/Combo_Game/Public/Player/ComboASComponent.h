// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Utils/ComboTypes.h"

#include "ComboASComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMBO_GAME_API UComboASComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init Data")
	UDataTable* ActionDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init Data")
	UComboASDataAsset* ComboASDataAsset;

	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	TSet<FComboNode> ComboNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	int32 NoneComboId = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TSet<FComboAction> Actions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float BasicPower = 10.0f;

private:
	UPROPERTY()
	int32 CurrentComboId = 0;

	UPROPERTY()
	FVector2D LastInputDirection;

	UPROPERTY()
	bool bLockInput = false;
	UPROPERTY()
	bool bLockRoll = false;

	UPROPERTY()
	int32 LockInputCount = 0;
	UPROPERTY()
	int32 LockRollCount = 0;

	UPROPERTY()
	bool bComboEnd = true;
	FTimerHandle ComboEndTimerHandle;

	UPROPERTY()
	float BonusPower = 0.0f;

public:
	UComboASComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Combo")
	int32 GetCurrentComboId();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void Roll();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool ComboInput(EComboInput Input);

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void LockComboInput(bool bLock);

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void LockRoll(bool bLock);

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo(float DelayTime);
	UFUNCTION(BlueprintPure, Category = "Combo")
	bool IsComboResetPending() const;

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void AddBonusPower(float Bonus);

	// 局内获取动作/添加节点
	UFUNCTION(BlueprintCallable, Category = "Combo RunTime")
	void AddComboAction(FComboAction NewAction);
	UFUNCTION(BlueprintCallable, Category = "Combo RunTime")
	bool AddComboNode(FComboNode NewNode);
	UFUNCTION(BlueprintCallable, Category = "Combo RunTime")
	bool DeleteComboNode(int32 ComboId);
	UFUNCTION(BlueprintCallable, Category = "Combo RunTime")
	FComboNode GetComboNodeByID(int32 ComboId) const;
	UFUNCTION(BlueprintCallable, Category = "Combo RunTime")
	void SaveComboNodes(TArray<FComboNode> InNodes);


protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	FComboNode ConstructComboNode(FInitComboNode InitNode);
	UFUNCTION()
	void InitializeAbilities();
	UFUNCTION()
	void HandleComboEndTimer();
	UFUNCTION()
	void CancelComboResetTimer();
	UFUNCTION()
	FVector GetMovementInputDirection() const;
	void RotateYawTowardDesired(AActor* Owner, const FVector& DesiredMoveDir, float DegreeLimitDeg);
	bool CheckPowerNeeded(FComboAction NextAction, float& Diff);
};
