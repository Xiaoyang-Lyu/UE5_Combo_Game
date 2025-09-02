// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ComboASComponent.generated.h"

UENUM(BlueprintType)
enum class EInputDirection : uint8
{
	Left,
	Right,
	Front,
	Back
};

UENUM(BlueprintType)
enum class EComboInput : uint8
{
	A,
	B,
};


USTRUCT(BlueprintType)
struct FComboAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Combo Node")
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditAnywhere, Category = "Combo Node")
	float DegreeLimit = 180.0f;

	bool operator==(const FComboAction& Other) const
	{
		return Ability == Other.Ability;
	}
};



USTRUCT(BlueprintType)
struct FComboNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Combo Node")
	int32 ComboId;

	UPROPERTY(EditAnywhere,Category = "Combo Node")
	FName ComboName;

	UPROPERTY(EditAnywhere, Category = "Combo Node")
	FComboAction Action;

	UPROPERTY(EditAnywhere, Category = "Combo Node")
	TMap<EComboInput, int32> NextComboMap;

	bool operator==(const FComboNode& Other) const
	{
		return ComboId == Other.ComboId;
	}
};

FORCEINLINE uint32 GetTypeHash(const FComboNode& Node)
{
	return ::GetTypeHash(Node.ComboId);
}

FORCEINLINE uint32 GetTypeHash(const FComboAction& Action)
{
	return ::GetTypeHash(Action.Ability);
}


UCLASS(BlueprintType)
class COMBO_GAME_API UComboASDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Normal Move")
	TMap<EInputDirection, TSubclassOf<UGameplayAbility>> RollAbilities;

	UPROPERTY(EditAnywhere, Category = "Base Combo")
	TArray<FComboNode> BaseComboNodes;
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COMBO_GAME_API UComboASComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	UComboASDataAsset* ComboASDataAsset;

	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	TSet<FComboNode> ComboNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	int32 NoneComboId = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Action")
	TSet<FComboAction> Actions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

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

	// 局内获取动作/添加节点
	UFUNCTION(BlueprintCallable, Category = "Combo RunTime")
	void AddComboAction(FComboAction& NewAction);
	UFUNCTION(BlueprintCallable, Category = "Combo RunTime")
	bool AddComboNode(FComboNode& NewNode);
	UFUNCTION(BlueprintCallable, Category = "Combo RunTime")
	bool DeleteComboNode(int32 ComboId);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void InitializeAbilities();
	UFUNCTION()
	void HandleComboEndTimer();
	UFUNCTION()
	void CancelComboResetTimer();
	UFUNCTION()
	FVector GetMovementInputDirection() const;
	void RotateYawTowardDesired(AActor* Owner, const FVector& DesiredMoveDir, float DegreeLimitDeg);
};
