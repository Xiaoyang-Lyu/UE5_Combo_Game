// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner()))
	{
		MovementComponent = Character->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MovementComponent.IsValid())
	{
		C_Velocity = MovementComponent->Velocity;
	}
}