// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class COMBO_GAME_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// Called when the owning actor is spawned
	virtual void NativeInitializeAnimation() override;

	// Called every frame
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Called when the owning actor is destroyed
	// virtual void NativeUninitializeAnimation() override;


protected:
	UPROPERTY(BlueprintReadWrite)
	FVector C_Velocity;

private:
	UPROPERTY()
	TWeakObjectPtr<class UCharacterMovementComponent> MovementComponent;
};
