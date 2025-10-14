// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "ComboTypes.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Node")
	int32 ActionId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Node")
	FName ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Node")
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Node")
	float DegreeLimit = 180.0f;

	bool operator==(const FComboAction& Other) const
	{
		return ActionId == Other.ActionId;
	}
};



USTRUCT(BlueprintType)
struct FComboNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Node")
	int32 ComboId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Node")
	FName ComboName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Node")
	FComboAction Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Node")
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
	return ::GetTypeHash(Action.ActionId);
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


// 用于配表，宝箱解锁的新动作节点之类的
USTRUCT(BlueprintType)
struct FComboNodesRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Candidate Combo Node")
	FComboAction Action;
};