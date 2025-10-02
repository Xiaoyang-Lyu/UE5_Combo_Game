// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/AnimUtils.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimTypes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// 内部小工具：对若干段根运动做“按时间顺序”的复合（保序乘法），考虑了缩放
static FORCEINLINE void AccumulateRootMotion(FTransform& InOutAccTM, const FTransform& DeltaTM)
{
	// 等价于 InOutAccTM = InOutAccTM * DeltaTM；使用 Accumulate 可正确处理缩放
	InOutAccTM.Accumulate(DeltaTM);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

FTransform UAnimUtils::ExtractRootMotionFromMontage(const UAnimMontage* Montage)
{
	FTransform AccTM = FTransform::Identity;
	if (!Montage)
	{
		return AccTM;
	}

	// 逐 Slot 逐 Segment 累加根运动（按蒙太奇的片段先后顺序）
	// 注意：如果不同 Slot 的时间存在重叠，这里会简单相加；典型“单 Slot”蒙太奇不会有问题
	for (const FSlotAnimationTrack& SlotTrack : Montage->SlotAnimTracks)
	{
		const FAnimTrack& AnimTrack = SlotTrack.AnimTrack;
		for (const FAnimSegment& Seg : AnimTrack.AnimSegments)
		{
			const UAnimSequenceBase* Seq = Seg.AnimReference;
			if (!Seq)
			{
				continue;
			}

			const float T0 = Seg.AnimStartTime;
			const float T1 = Seg.AnimEndTime;

			// 从 Sequence 自身时间轴提取根运动（0→末帧或任意时间段）
			const FTransform DeltaTM = Seq->ExtractRootMotionFromRange(T0, T1);
			AccumulateRootMotion(AccTM, DeltaTM);
		}
	}

	return AccTM;
}

float UAnimUtils::GetMontageHorizontalDistance2D(const UAnimMontage* Montage)
{
	const FTransform DeltaTM = ExtractRootMotionFromMontage(Montage);
	const FVector Delta = DeltaTM.GetTranslation();
	return FVector(Delta.X, Delta.Y, 0.f).Size();
}

FRotator UAnimUtils::GetMontageNetRotation(const UAnimMontage* Montage, bool bYawOnly /*=true*/)
{
	const FTransform DeltaTM = ExtractRootMotionFromMontage(Montage);
	FRotator R = DeltaTM.GetRotation().Rotator();

	if (bYawOnly)
	{
		return FRotator(0.f, R.Yaw, 0.f);
	}
	return R;
}

FTransform UAnimUtils::ExtractRootMotionFromSequence(const UAnimSequenceBase* Sequence, float StartTime, float EndTime)
{
	if (!Sequence)
	{
		return FTransform::Identity;
	}

	if (EndTime < StartTime)
	{
		Swap(StartTime, EndTime);
	}

	return Sequence->ExtractRootMotionFromRange(StartTime, EndTime);
}

void UAnimUtils::ComputeWarpTargetFromMontage(
	const UAnimMontage* Montage,
	const FVector& StartLocation,
	const FVector& DesiredDirection,
	FVector& OutTargetLocation,
	FRotator& OutTargetRotation
)
{
	// 1) 目标朝向：面向“期望方向”。若方向为零，用世界 X 轴。
	FVector Dir2D = FVector(DesiredDirection.X, DesiredDirection.Y, 0.f);
	if (!Dir2D.Normalize())
	{
		Dir2D = FVector(1.f, 0.f, 0.f);
	}
	OutTargetRotation = Dir2D.Rotation();

	// 2) 位移距离：用“蒙太奇的水平总位移”
	const float Dist2D = GetMontageHorizontalDistance2D(Montage);

	// 3) 目标位置：起点 + 方向 * 距离
	OutTargetLocation = StartLocation + Dir2D * Dist2D;
}
