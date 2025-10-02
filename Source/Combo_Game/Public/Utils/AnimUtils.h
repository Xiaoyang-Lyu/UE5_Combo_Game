#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimUtils.generated.h"

class UAnimMontage;
class UAnimSequenceBase;

/**
 * 动画根运动工具：从 Montage/Sequence 提取 0→末帧的最终位移与朝向
 * - ExtractRootMotionFromMontage：复合 Montage 全部片段的根运动（Transform）
 * - GetMontageHorizontalDistance2D：水平（XY）总位移长度
 * - GetMontageNetRotation：总旋转（默认只取 Yaw）
 * - ExtractRootMotionFromSequence：序列在任意时间段的根运动（Transform）
 * - ComputeWarpTargetFromMontage：给出起点与“期望方向”，用蒙太奇的水平位移推算 Warping 目标点与目标朝向
 */
UCLASS()
class COMBO_GAME_API UAnimUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 复合 Montage 全部 Slot/Segment 的根运动（0→末帧），返回 Delta Transform（位移+旋转+缩放） */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim|Utils")
	static FTransform ExtractRootMotionFromMontage(const UAnimMontage* Montage);

	/** 提取 Montage 的“水平”总位移长度（忽略 Z） */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim|Utils")
	static float GetMontageHorizontalDistance2D(const UAnimMontage* Montage);

	/** 提取 Montage 的总旋转。bYawOnly=true 时只返回 Yaw（其余轴为0） */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim|Utils")
	static FRotator GetMontageNetRotation(const UAnimMontage* Montage, bool bYawOnly = true);

	/** 提取任意 Sequence 在 [StartTime, EndTime] 的根运动（Transform） */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim|Utils")
	static FTransform ExtractRootMotionFromSequence(const UAnimSequenceBase* Sequence, float StartTime, float EndTime);

	/**
	 * 用“蒙太奇的水平总位移” + “期望方向” 计算一个 Warping 目标：
	 *   OutTargetLocation = StartLocation + Normalized(DesiredDirection) * HorizontalDistance
	 *   OutTargetRotation = Facing(DesiredDirection)
	 * DesiredDirection 可以不是单位向量；若为零则默认 X 轴正向。
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Anim|Utils")
	static void ComputeWarpTargetFromMontage(
		const UAnimMontage* Montage,
		const FVector& StartLocation,
		const FVector& DesiredDirection,
		FVector& OutTargetLocation,
		FRotator& OutTargetRotation
	);
};
