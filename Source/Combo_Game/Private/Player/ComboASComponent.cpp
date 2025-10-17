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
		if (Node.Action.Ability)
		{
			Actions.Add(Node.Action);
			FGameplayAbilitySpec AbilitySpec(Node.Action.Ability, 1, Node.ComboId);
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
	if (bLockRoll) return;

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

	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(*ComboASDataAsset->RollAbilities.Find(EInputDirection::Front));

	for (UGameplayAbility* Instance : Spec->GetAbilityInstances())
	{
		if (Instance && Instance->CanBeCanceled())
		{
			CancelAbility(Instance);
		}
	}

    TryActivateAbilityByClass(*ComboASDataAsset->RollAbilities.Find(EInputDirection::Front));
	CurrentComboId = NoneComboId;
}

bool UComboASComponent::ComboInput(EComboInput Input)
{
	if (bLockInput) return false;

	if (FComboNode* CurrentNode = ComboNodes.Find(FComboNode(GetCurrentComboId())))
	{
		// 先找当前combo的后续
		int32* NextId = CurrentNode->NextComboMap.Find(Input);

		if (NextId != nullptr)
		{
			float PowerDiff = 0.f;
			FComboAction NextAction = GetComboNodeByID(*NextId).Action;
			if (!CheckPowerNeeded(NextAction, PowerDiff))
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Not enough power for next action. Need %.1f more power."), PowerDiff));
				NextId = nullptr;
			}
		}

		bool ContinueCombo = NextId != nullptr; // 暂时还没用

		// 若当前combo没有后续，则尝试从初始combo开始找
		if (!NextId && CurrentComboId != NoneComboId)
		{
			NextId = ComboNodes.Find(FComboNode(NoneComboId))->NextComboMap.Find(Input);
		}

		if (NextId != nullptr)
		{
			float PowerDiff = 0.f;
			FComboAction NextAction = GetComboNodeByID(*NextId).Action;
			if (!CheckPowerNeeded(NextAction, PowerDiff))
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Not enough power for next action. Need %.1f more power."), PowerDiff));
				NextId = nullptr;
				return false;
			}
		}

		if (NextId != nullptr)
		{
			CurrentComboId = *NextId;

			if (FComboNode* NextNode = ComboNodes.Find(FComboNode(CurrentComboId)))
			{
				if (IsValid(NextNode->Action.Ability))
				{
					bool Succees =  TryActivateAbilityByClass(*NextNode->Action.Ability);
					if (Succees)
					{
						const FVector DesiredMoveDir = GetMovementInputDirection();
						RotateYawTowardDesired(GetOwner(), DesiredMoveDir, NextNode->Action.DegreeLimit);
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
	//print
	GEngine->AddOnScreenDebugMessage(-1, 0.4f, FColor::Yellow, FString::Printf(TEXT("LockInputCount: %d, bLockInput: %d"), LockInputCount, bLockInput));
}


void UComboASComponent::LockRoll(bool bLock)
{
	if (bLock)
	{
		LockRollCount++;
		bLockRoll = true;
	}
	else
	{
		LockRollCount--;
		if (LockRollCount <= 0)
		{
			LockRollCount = 0;
			bLockRoll = false;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.4f, FColor::Yellow, FString::Printf(TEXT("LockRollCount: %d, bLockRoll: %d"), LockRollCount, bLockRoll));
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

void UComboASComponent::AddComboAction(FComboAction NewAction)
{
	if (!IsValid(NewAction.Ability))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UComboASComponent] AddComboAction failed: Ability is null"));
		return;
	}

	if(Actions.Contains(NewAction))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UComboASComponent] AddComboAction failed: Action already exists"));
		return;
	}

	FGameplayAbilitySpec AbilitySpec(NewAction.Ability, 1, Actions.Num());
	GiveAbility(AbilitySpec);
	Actions.Add(NewAction);
}

bool UComboASComponent::AddComboNode(FComboNode NewNode)
{
	if (NewNode.ComboId == NoneComboId)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UComboASComponent] AddComboNode failed: ComboId cannot be NoneComboId"));
		return false;
	}

	if (ComboNodes.Contains(NewNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UComboASComponent] AddComboNode failed: ComboId already exists"));
		return false;
	}

	if (!IsValid(NewNode.Action.Ability))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UComboASComponent] AddComboNode failed: Ability is null"));
		return false;
	}

	ComboNodes.Add(NewNode);
	return true;
}

bool UComboASComponent::DeleteComboNode(int32 ComboId)
{
	if (ComboId == NoneComboId)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UComboASComponent] DeleteComboNode failed: ComboId cannot be NoneComboId"));
		return false;
	}

	if (FComboNode* Node = ComboNodes.Find(FComboNode(ComboId)))
	{
		ComboNodes.Remove(*Node);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UComboASComponent] DeleteComboNode failed: ComboId not found"));
		return false;
	}

}

FComboNode UComboASComponent::GetComboNodeByID(int32 ComboId) const
{
	if (const FComboNode* Node = ComboNodes.Find(FComboNode(ComboId)))
	{
		return *Node;
	}
	return FComboNode();
}

void UComboASComponent::SaveComboNodes(TArray<FComboNode> InNodes)
{
	//for each node in InNodes, if ComboId exists, update it, else add it
	for (const FComboNode& Node : InNodes)
	{
		if (Node.ComboId == NoneComboId)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UComboASComponent] SaveComboNodes skipped: ComboId cannot be NoneComboId"));
			continue;
		}

		if (FComboNode* ExistingNode = ComboNodes.Find(FComboNode(Node.ComboId)))
		{
			*ExistingNode = Node;
		}
		else
		{
			if (IsValid(Node.Action.Ability))
			{
				ComboNodes.Add(Node);
			}
		}
	}
}

bool UComboASComponent::CheckPowerNeeded(FComboAction NextAction, float& Diff)
{
	if (CurrentComboId == NoneComboId)
	{
		Diff = NextAction.PowerNeeded - BasicPower;
		return Diff <= 0;
	}
	FComboNode* CurrentNode = ComboNodes.Find(FComboNode(GetCurrentComboId()));
	FComboAction CurrentAction = CurrentNode->Action;
	
	// Check Direction
	TSet<EActionDirection> BuffedDirections;
	for (EActionDirection Dir : CurrentAction.ActionDirections)
	{
		if (Dir == EActionDirection::Left)
		{
			BuffedDirections.Add(EActionDirection::Right);
		}
		else if (Dir == EActionDirection::Right)
		{
			BuffedDirections.Add(EActionDirection::Left);
		}
		else if (Dir == EActionDirection::Up)
		{
			BuffedDirections.Add(EActionDirection::Down);
		}
		else if (Dir == EActionDirection::Down)
		{
			BuffedDirections.Add(EActionDirection::Up);
		}
	}

	bool bBuffed = false;
	for (EActionDirection Dir : NextAction.ActionDirections)
	{
		if (BuffedDirections.Contains(Dir))
		{
			bBuffed = true;
			break;
		}
	}

	if (bBuffed)
	{
		Diff = NextAction.PowerNeeded - (BasicPower + CurrentAction.PowerBuffed);
		return Diff <= 0;
	}
	else
	{
		Diff = NextAction.PowerNeeded - BasicPower;
		return Diff <= 0;
	}
}