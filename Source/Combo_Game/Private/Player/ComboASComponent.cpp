// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ComboASComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Animation/AnimMontage.h"
#include "Ability/PlayMontageOnSlotAbility.h"
#include "Utils/AnimUtils.h" 

UComboASComponent::UComboASComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UComboASComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeAbilities();
	CurrentComboId = NoneComboId;
}

void UComboASComponent::InitializeAbilities()
{
	for (auto& Elem : ComboASDataAsset->RollAbilities)
	{
		if (Elem.Value)
		{
			FGameplayAbilitySpec AbilitySpec(Elem.Value, 1, static_cast<int32>(Elem.Key));
			GiveAbility(AbilitySpec);
		}
	}

	for (const FComboNode& Node : ComboASDataAsset->BaseComboNodes)
	{
		ComboNodes.Add(Node);
		if (Node.Ability)
		{
			FGameplayAbilitySpec AbilitySpec(Node.Ability, 1, Node.ComboId);
			GiveAbility(AbilitySpec);
		}
	}
}


void UComboASComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


int32 UComboASComponent::GetCurrentComboId()
{
	if (bComboEnd)
	{
		CurrentComboId = NoneComboId;
	}
	return CurrentComboId;
}


FVector UComboASComponent::GetMovementInputDirection() const
{
	FVector DesiredMoveDir = FVector::ZeroVector;

	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
	{
		return DesiredMoveDir;
	}
	ACharacter* Character = Cast<ACharacter>(PawnOwner);
	APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());

	if (PC && MoveAction)
	{
		if (UEnhancedPlayerInput* EPI = Cast<UEnhancedPlayerInput>(PC->PlayerInput))
		{
			const FInputActionValue Val = EPI->GetActionValue(MoveAction); // 期望是 Axis2D
			const FVector2D Axis = Val.IsNonZero() ? Val.Get<FVector2D>() : FVector2D::ZeroVector;

			if (!Axis.IsNearlyZero())
			{
				FRotator ControlRot = PC->GetControlRotation();
				FVector RightVector = UKismetMathLibrary::GetRightVector(ControlRot);
				FVector ForwardVector = UKismetMathLibrary::GetForwardVector(ControlRot);
				DesiredMoveDir = (ForwardVector * Axis.Y + RightVector * Axis.X).GetSafeNormal2D();
			}
		}
	}

	if (DesiredMoveDir.IsNearlyZero())
	{
		if (PC)
		{
			DesiredMoveDir = FRotator(0.f, PC->GetControlRotation().Yaw, 0.f).Vector();
		}
		else
		{
			DesiredMoveDir = PawnOwner->GetActorForwardVector();
		}
	}
	
	return DesiredMoveDir;
}


void UComboASComponent::RotateYawTowardDesired(AActor* Owner, const FVector& DesiredMoveDir, float DegreeLimitDeg)
{
	if (!Owner || DesiredMoveDir.IsNearlyZero())
	{
		return;
	}

	const float CurrYaw = Owner->GetActorRotation().Yaw;

	const float TargetYaw = DesiredMoveDir.Rotation().Yaw;

	const float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrYaw, TargetYaw);

	const float Applied = FMath::Clamp(DeltaYaw, -DegreeLimitDeg, DegreeLimitDeg);

	FRotator NewRot = Owner->GetActorRotation();
	NewRot.Yaw = CurrYaw + Applied;
	Owner->SetActorRotation(NewRot);
}


void UComboASComponent::Roll()
{
	// TODO: 之后bLockRoll，翻滚可能可以比连招更早取消当前动作
	if (bLockInput) return;

    if (!ComboASDataAsset)
    {
        return;
    }

    APawn* PawnOwner = Cast<APawn>(GetOwner());
    if (!PawnOwner)
    {
        return;
    }
    ACharacter* Character = Cast<ACharacter>(PawnOwner);

	FVector DesiredMoveDir = GetMovementInputDirection();

	//rotate to desired dir
	FRotator TargetRot = DesiredMoveDir.Rotation();
	Character->SetActorRotation(TargetRot);

    TryActivateAbilityByClass(*ComboASDataAsset->RollAbilities.Find(EInputDirection::Front));
	CurrentComboId = NoneComboId;
}

bool  UComboASComponent::ComboInput(EComboInput Input)
{
	if (bLockInput) return false;

	if (FComboNode* CurrentNode = ComboNodes.Find(FComboNode(GetCurrentComboId())))
	{
		int32* NextId = CurrentNode->NextComboMap.Find(Input);
		if (!NextId && CurrentComboId != NoneComboId)
		{
			NextId = ComboNodes.Find(FComboNode(NoneComboId))->NextComboMap.Find(Input);
		}

		if (NextId != nullptr)
		{
			CurrentComboId = *NextId;

			if (FComboNode* NextNode = ComboNodes.Find(FComboNode(CurrentComboId)))
			{
				if (NextNode->Ability)
				{
					bool Succees =  TryActivateAbilityByClass(*NextNode->Ability);
					if (Succees)
					{
						const FVector DesiredMoveDir = GetMovementInputDirection();
						RotateYawTowardDesired(GetOwner(), DesiredMoveDir, NextNode->DegreeLimit);
						bComboEnd = false;
						CancelComboResetTimer();
						return true;
					}
				}
			}
		}
	}
	return false;
}

void UComboASComponent::LockComboInput(bool bLock)
{
	if (bLock)
	{
		LockInputCount++;
		bLockInput = true;
	}
	else
	{
		LockInputCount--;
		if (LockInputCount <= 0)
		{
			LockInputCount = 0;
			bLockInput = false;
		}
	}
}


void UComboASComponent::ResetCombo(float DelayTime)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		bComboEnd = true;
		return;
	}

	World->GetTimerManager().ClearTimer(ComboEndTimerHandle);

	if (DelayTime <= 0.f)
	{
		bComboEnd = true;
		return;
	}

	World->GetTimerManager().SetTimer(
		ComboEndTimerHandle,
		this,
		&UComboASComponent::HandleComboEndTimer,
		DelayTime,
		false
	);
}

void UComboASComponent::CancelComboResetTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ComboEndTimerHandle);
	}
}

bool UComboASComponent::IsComboResetPending() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetTimerManager().IsTimerActive(ComboEndTimerHandle) : false;
}

void UComboASComponent::HandleComboEndTimer()
{
	bComboEnd = true;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ComboEndTimerHandle);
	}
}