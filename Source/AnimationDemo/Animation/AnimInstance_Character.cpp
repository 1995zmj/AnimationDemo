// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance_Character.h"
#include "AnimationDemo/Character/CharacterInformationInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "VisualLogger/VisualLogger.h"

void UAnimInstance_Character::OnInitializeAnimation()
{
	auto PawnOwner = TryGetPawnOwner();
	if (IsValid(PawnOwner))
	{
		auto LocalCharacter = Cast<ACharacter>(PawnOwner);
		if (IsValid(LocalCharacter))
		{
			Character = LocalCharacter;
		}
	}

	// 一些配置参数
	SmoothedAimingRotationInterpSpeed = 10.0f;
	VelocityBlendInterpSpeed = 12.0f;
	GroundedLeanInterpSpeed = 4.0f;
	AnimatedWalkSpeed = 150;
	AnimatedRunSpeed = 350;
	AnimatedCrouchSpeed = 150;
	AnimatedSprintSpeed = 600;
	TriggerPivotSpeedLimit = 200;


	//
	TurnCheckMinAngle = 45.0f;
	AimYawRateLimit = 50.0f;
	MinAngleDelay = 0.75f;
	MaxAngleDelay = 0.0f;
	// 原地转头旋转
	RotateMinThreshold = -50.0f;
	RotateMaxThreshold = 50.0f;
	AimYawRateMinRange = 90.0f;
	AimYawRateMaxRange = 270.0f;
	MinPlayRate = 1.15f;
	MaxPlayRate = 3.0f;
	Turn180Threshold = 130;

	InAirLeanInterpSpeed = 4.0f;

	//IK Config
	FootHeight = 13.5f;
	IK_TraceDistanceAboveFoot = 50.0f;
	IK_TraceDistanceBelowFoot = 45.0f;
	
	// 默认参数
	ShouldMove = false;
}

void UAnimInstance_Character::OnUpdateAnimation(float DeltaTimeX)
{
	MyDeltaTimeX = DeltaTimeX;

	if (MyDeltaTimeX <= 0 || !IsValid(Character))
	{
		return;
	}
	
	UpdateCharacterInfo();
	UpdateAimingValues();
	UpdateLayerValues();
	UpdateFootIK();


	
	UE_VLOG(this, "character", Verbose,
	TEXT("Name (%s)\n key Velocity (%s)\n  key Speed (%f)\n key Acceleration (%s)\n"),
	*GetName(),
	*Velocity.ToString(),
	Speed,
	*Acceleration.ToString()
	);
	bool LastShouldMove = ShouldMove;

	switch (MovementState)
	{
	case EMovementState_ZMJ::Grounded:
		ShouldMove = ShouldMoveCheck();
		if (ShouldMove)
		{
			// 移动
			UpdateMovementValues();
			UpdateRotationValues();
		}
		else
		{
			// 停止
			//
			/*if (CanRotateInPlace())
			{
				RotateInPlaceCheck();
			}
			else
			{
				Rotate_L = false;
				Rotate_R = false;
			}*/

			//
			if (CanTurnInPalce())
			{
				TurnInPlaceCheck();
			}
			else
			{
				ElapsedDelayTime = 0.0;
			}
				
		}
		if (LastShouldMove != ShouldMove)
		{
			if (ShouldMove)
			{
				// 开始移动
				ElapsedDelayTime = 0.0f;
				Rotate_L = false;
				Rotate_R = false;
			}
			else
			{
				// 开始停止
			}
		}
		
		break;
	case EMovementState_ZMJ::InAir:
		UpdateInAirValues();
		break;
	}

	UE_VLOG(this, "AnimInstace", Verbose,
	TEXT("Name (%s)/n key StrdeBlend (%f)/n key WalkRunBlend (%f)/n"),
	*GetName(),
	StrdeBlend,
	WalkRunBlend
	);
}

void UAnimInstance_Character::PlayTransition(FDynamicMontageParams_ZMJ Parameters)
{
	PlaySlotAnimationAsDynamicMontage(Parameters.Animation,FName("Grounded Slot"),
		Parameters.BlendInTime,Parameters.BlendOutTime,Parameters.PlayRate,1,0.0f,Parameters.StartTime);
}

void UAnimInstance_Character::UpdateCharacterInfo()
{
	if (Character->Implements<UCharacterInformationInterface>())
	{
		ICharacterInformationInterface* Interface = Cast<ICharacterInformationInterface>(Character);
		FCharacterInformation_ZMJ OutCharacterInformation;
		FCharacterStates_ZMJ OutCharacterStates;
		Interface->GetEssentialValues(OutCharacterInformation);
		Interface->GetCurrentStates(OutCharacterStates);

		Velocity			= OutCharacterInformation.Velocity				;
		Acceleration		= OutCharacterInformation.Acceleration			;
		MovementInput		= OutCharacterInformation.MovementInput			;
		IsMoving			= OutCharacterInformation.IsMoving				;
		HasMovementInput	= OutCharacterInformation.HasMovementInput		;
		Speed				= OutCharacterInformation.Speed					;
		MovementInputAmount	= OutCharacterInformation.MovementInputAmount	;
		AimingRotation		= OutCharacterInformation.AimingRotation		;
		AimYawRate			= OutCharacterInformation.AimYawRate			;

		PawnMovementMode	= OutCharacterStates.PawnMovementMode	;
		MovementState		= OutCharacterStates.MovementState		;
		PrevMovementState	= OutCharacterStates.PrevMovementState	;
		MovementAction		= OutCharacterStates.MovementAction		;
		RotationMode		= OutCharacterStates.RotationMode		;
		ActualGait			= OutCharacterStates.ActualGait			;
		ActualStance		= OutCharacterStates.ActualStance		;
		ViewMode			= OutCharacterStates.ViewMode			;
		OverlaySate			= OutCharacterStates.OverlaySate		;
	}
}

void UAnimInstance_Character::UpdateAimingValues()
{
	// Interp瞄准旋转值以实现平滑的瞄准旋转变化。在计算角度之前插入旋转确保该值不受actor旋转变化的影响，允许缓慢的瞄准旋转变化与快速的actor旋转变化。
	SmoothedAimingRotation = FMath::RInterpTo(SmoothedAimingRotation,AimingRotation,MyDeltaTimeX,SmoothedAimingRotationInterpSpeed);

	// 通过计算目标旋转与动作者旋转之间的差值，计算目标角和平滑目标角。
	auto LocalRotation = AimingRotation - Character->GetActorRotation();
	LocalRotation.Normalize();
	AimingAngle.X = LocalRotation.Yaw;
	AimingAngle.Y = LocalRotation.Pitch;
	LocalRotation = SmoothedAimingRotation - Character->GetActorRotation();
	LocalRotation.Normalize();
	SmoothedAimingAngle.X = LocalRotation.Yaw;
	SmoothedAimingAngle.Y = LocalRotation.Pitch;

	if (RotationMode != ERotationMode_ZMJ::VelocityDirection)
	{
		// 夹紧瞄准俯仰角到1到0的范围内用于垂直瞄准扫瞄。
		AimSweepTime = FMath::GetMappedRangeValueClamped(FVector2D(-90.0f, 90.0f), FVector2D(1.0f, 0.0f), AimingAngle.Y);

		// 使用瞄准偏航角除以脊柱+骨盆骨的数量，以获得脊柱旋转的数量，以保持面向相机的方向。
		SpineRotation = FRotator::ZeroRotator;
		SpineRotation.Yaw = (AimingAngle.X / 4.0f);
	}
	else
	{
		if (HasMovementInput)
		{
			// 获取移动输入旋转和角色旋转之间的增量，并将其映射到0-1的范围内。这个值用于目标偏移行为，使角色朝向移动输入。
			LocalRotation = MovementInput.ToOrientationRotator() - Character->GetActorRotation();
			LocalRotation.Normalize();
			auto LocalValue = FMath::GetMappedRangeValueClamped(FVector2D(-180.0f, 180.0f), FVector2D(0.0f, 1.0f), LocalRotation.Yaw);
			InputYawOffsetTime = FMath::FInterpTo(InputYawOffsetTime,LocalValue,MyDeltaTimeX,InputYawOffsetTime);
		}
	}

	// 将瞄准偏航角分成3个独立的偏航时间。这3个值用于目标偏移行为，以改善完全围绕角色旋转时的目标偏移的混合。这允许你保持瞄准的响应，但仍然平稳地从左到右或从右到左混合。
	LeftYawTime = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 180.0f), FVector2D(0.5f, 0.0f), FMath::Abs(SmoothedAimingAngle.X));
	RightYawTime = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 180.0f), FVector2D(0.5f, 1.0f), FMath::Abs(SmoothedAimingAngle.X));
	ForwardYawTime = FMath::GetMappedRangeValueClamped(FVector2D(-180.0f, 180.0f), FVector2D(0.0f, 1.0f), SmoothedAimingAngle.X);

}

void UAnimInstance_Character::UpdateLayerValues()
{
	// 通过获得与“目标偏移遮罩”相反的权重来获得“目标偏移”权重。
	Enable_AimOffset = FMath::Lerp(1.0f,0.0f,GetCurveValue(TEXT("Mask_AimOffset")));

	// 设置基本姿势权重
	BasePose_N = GetCurveValue(TEXT("BasePose_N"));
	BasePose_CLF = GetCurveValue(TEXT("BasePose_CLF"));

	// 设置每个身体部位的添加量权重
	SpineAdd	= GetCurveValue(TEXT("Layering_Spine_Add"));
	HeadAdd		= GetCurveValue(TEXT("Layering_Head_Add"));
	ArmLAdd		= GetCurveValue(TEXT("Layering_Arm_L_Add"));
	ArmRAdd		= GetCurveValue(TEXT("Layering_Arm_R_Add"));

	// 设置臂是否应该融入网格空间或局部空间。网格空间权重将始终为1，除非局部空间(LS)曲线完全加权。
	ArmL_LS		= GetCurveValue(TEXT("Layering_Arm_L_LS"));
	ArmR_LS		= GetCurveValue(TEXT("Layering_Arm_R_LS"));
	ArmL_MS		= FMath::FloorToInt(ArmL_LS) - 1;
	ArmR_MS		= FMath::FloorToInt(ArmR_LS) - 1;

	// 设置手动覆盖权重
	HandL		= GetCurveValue(TEXT("Layering_Hand_L"));
	HandR		= GetCurveValue(TEXT("Layering_Hand_R"));

	// 混合并设置手IK权重，以确保仅在手臂层允许的情况下对其进行加权。
	Enable_HandIK_L = FMath::Lerp(0.0f,GetCurveValue("Enable_HandIK_L"),GetCurveValue("Layering_Arm_L"));
	Enable_HandIK_R = FMath::Lerp(0.0f,GetCurveValue("Enable_HandIK_R"),GetCurveValue("Layering_Arm_R"));
}

void UAnimInstance_Character::UpdateFootIK()
{
	SetFootLocking(FName("Enable_FootIK_L"),FName("FootLock_L"),FName("ik_foot_l"),FootLock_L_Alpha,FootLock_L_Location,FootLock_L_Rotation);
	SetFootLocking(FName("Enable_FootIK_R"),FName("FootLock_R"),FName("ik_foot_r"),FootLock_R_Alpha,FootLock_R_Location,FootLock_R_Rotation);
	FVector FootOffset_L_Target;
	FVector FootOffset_R_Target;
	switch (MovementState)
	{
	case EMovementState_ZMJ::None:
	case EMovementState_ZMJ::Grounded:
	case EMovementState_ZMJ::Mantling:
		// 不在空中时更新所有“脚锁定”和“脚偏移”值
		SetFootOffsets(FName("Enable_FootIK_L"),FName("ik_foot_l"),FName("root"),FootOffset_L_Target,FootOffset_L_Location,FootOffset_L_Rotation);
		SetFootOffsets(FName("Enable_FootIK_R"),FName("ik_foot_r"),FName("root"),FootOffset_R_Target,FootOffset_R_Location,FootOffset_R_Rotation);

		SetPelvisIKOffset(FootOffset_L_Target,FootOffset_R_Target);
		break;
	case EMovementState_ZMJ::InAir:
		SetPelvisIKOffset(FVector::ZeroVector,FVector::ZeroVector);
		ResetIKOffsets();
		break;
	}
}

void UAnimInstance_Character::UpdateMovementValues()
{
	// Interp并设置速度混合。
	VelocityBlend = InterpVelocityBlend(VelocityBlend,CalculateVelocityBlend(),VelocityBlendInterpSpeed,MyDeltaTimeX);

	// 设置对角刻度量。
	// DiagonalScaleAmount = CalculateDiagonalScaleAmount();

	// 设置相对加速度量并插入倾斜量。
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();
	FLeanAmount_ZMJ LocalLeanAmount;
	LocalLeanAmount.FB = RelativeAccelerationAmount.X;
	LocalLeanAmount.LR = RelativeAccelerationAmount.Y;
	LeanAmount = InterpLeanAmount(LeanAmount,LocalLeanAmount,GroundedLeanInterpSpeed,MyDeltaTimeX);
	
	WalkRunBlend = CalculateWalkRunBlend();
	// 通过速度确定步伐
	StrdeBlend = CalculateStrideBlend();
	
	StandingPlayRate = CalculateStandingPlayRate();
	CrouchingPlayRate = CalculateCrouchingPlayRate();
	UE_VLOG(this, "AnimInstace", Verbose,
	TEXT("Name (%s)/n key LeanAmount FB(%f)LR(%f)"),
	*GetName(),
	LeanAmount.FB,
	LeanAmount.LR
	);
}

void UAnimInstance_Character::UpdateRotationValues()
{
	// 设定移动方向
	MovementDirection = CalculateMovementDirection();

	UE_VLOG(this, "AnimInstace", Verbose,
	TEXT("Name (%s)/n key MovementDirection (%s)"),
	*GetName(),
	*StaticEnum<EMovementDirection_ZMJ>()->GetNameStringByValue((int64)MovementDirection)
	);
	
	// 设置偏航偏移量。这些值影响动画中的“YawOffset”曲线，用于偏移字符的旋转，以获得更自然的移动。曲线允许对每个运动方向的偏移行为进行精细控制。
	auto LocalRotation = Velocity.ToOrientationRotator() - Character->GetControlRotation();
	LocalRotation.Normalize();
	auto FB = GetVectorValue(YawOffset_FB,LocalRotation.Yaw);
	auto LR = GetVectorValue(YawOffset_LR,LocalRotation.Yaw);
	FYaw = FB.X;
	BYaw = FB.Y;
	LYaw = LR.X;
	RYaw = LR.Y;
}

void UAnimInstance_Character::UpdateInAirValues()
{
	// 更新下降速度。只在空中设置这个值可以让你在AnimGraph中使用它来计算着陆强度。否则，着陆时Z速度将返回0。
	FallSpeed = Velocity.Z;
	// 设置土地预测权重。
	LandPrediction = CalculateLandPrediction();

	// Interp并设置In Air Lean Amount
	LeanAmount = InterpLeanAmount(LeanAmount,CalculateInAirLeanAmount(),InAirLeanInterpSpeed,MyDeltaTimeX);
}

bool UAnimInstance_Character::ShouldMoveCheck()
{
	return Speed > 150 || (IsMoving && HasMovementInput);
}

// 仅当角色以第三人称朝向摄影机并且“启用过渡”曲线已完全加权时，才执行“原地转弯”检查。Enable_Transition曲线在AnimBP的某些状态下进行修改，以便角色只能在这些状态下转动。
bool UAnimInstance_Character::CanTurnInPalce()
{
	return RotationMode == ERotationMode_ZMJ::LookingDirection && ViewMode == EViewMode_ZMJ::ThirdPerson && GetCurveValue(FName("Enable_Transition")) > 0.99;
}

// 只有当角色在第三人称视角下面向摄像机，以及“启用过渡”曲线完全加权时，才执行原地转弯检查。Enable_Transition曲线在AnimBP的某些状态下被修改，以便角色只能在这些状态下转弯。
bool UAnimInstance_Character::CanRotateInPlace()
{
	return RotationMode == ERotationMode_ZMJ::Aiming || ViewMode == EViewMode_ZMJ::FirstPerson;
}

bool UAnimInstance_Character::CanDynamicTransition()
{
	return GetCurveValue(FName("Enable_Transition")) == 1.0f;
}

void UAnimInstance_Character::TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool OverrideCurrent)
{
	// 步骤1：设置转角
	FRotator Delta = TargetRotation -Character->GetActorRotation();
	Delta.Normalize();
	auto TurnAngle = Delta.Yaw;

	// 步骤2：根据转弯角度和姿势选择转弯资源
	auto TargetTurnAsset = N_TurnIP_L90;
	if (FMath::Abs(TurnAngle) < Turn180Threshold)
	{
		if (TurnAngle < 0.0)
		{
			switch (ActualStance)
			{
			case EStance_ZMJ::Standing:
				TargetTurnAsset = N_TurnIP_L90;
				break;
			case EStance_ZMJ::Crouching:
				TargetTurnAsset = CLF_TurnIP_L90;
				break;
			}
		}
		else
		{
			switch (ActualStance)
            {
            case EStance_ZMJ::Standing:
            	TargetTurnAsset = N_TurnIP_R90;
            	break;
            case EStance_ZMJ::Crouching:
            	TargetTurnAsset = CLF_TurnIP_R90;
            	break;
            }
		}
	}
	else
	{
		if (TurnAngle < 0.0)
		{
			switch (ActualStance)
			{
			case EStance_ZMJ::Standing:
				TargetTurnAsset = N_TurnIP_L180;
				break;
			case EStance_ZMJ::Crouching:
				TargetTurnAsset = CLF_TurnIP_L180;
				break;
			}
		}
		else
		{
			switch (ActualStance)
			{
			case EStance_ZMJ::Standing:
				TargetTurnAsset = N_TurnIP_R180;
				break;
			case EStance_ZMJ::Crouching:
				TargetTurnAsset = CLF_TurnIP_R180;
				break;
			}
		}
	}

	// 步骤3:如果目标回合动画没有播放或设置为被覆盖，将回合动画作为动态蒙太奇播放。
	if (OverrideCurrent || !IsPlayingSlotAnimation(TargetTurnAsset.Animation,TargetTurnAsset.SlotName))
	{
		PlaySlotAnimationAsDynamicMontage(TargetTurnAsset.Animation,TargetTurnAsset.SlotName,0.2,0.2,
			TargetTurnAsset.PlayRate * PlayRateScale,1,0.0f,StartTime);
	}

	//  步骤4:调整旋转量(在动画中调整)以补偿转角(如果允许)和游戏率。
	if (TargetTurnAsset.ScaleTurnAngle)
	{
		RotationScale = (TurnAngle / TargetTurnAsset.AnimatedAngle) * TargetTurnAsset.PlayRate * PlayRateScale;
	}
	else
	{
		RotationScale = TargetTurnAsset.PlayRate * PlayRateScale;
	}
}

void UAnimInstance_Character::TurnInPlaceCheck()
{
	// 步骤1：检查瞄准角度是否超出转弯检查最小角度，以及瞄准偏航率是否低于瞄准偏航速率限制。如果是，则开始计算已用延迟时间。如果没有，则重置已用延迟时间。这确保了在转向到位之前，条件在持续一段时间内保持正确。

	auto AbsAimingAngleX = FMath::Abs(AimingAngle.X);
	if (AbsAimingAngleX > TurnCheckMinAngle && AimYawRate < AimYawRateLimit)
	{
		ElapsedDelayTime = ElapsedDelayTime + MyDeltaTimeX;
		// 步骤2：检查经过的延迟时间是否超过设置的延迟（映射到转角范围）。如果是，则触发“原地转弯”。

		auto LocalTemp = FMath::GetMappedRangeValueClamped(FVector2D(TurnCheckMinAngle, 180.0f), FVector2D(MinAngleDelay, MaxAngleDelay), AbsAimingAngleX);
		if (ElapsedDelayTime > LocalTemp)
		{
			auto LocalRotation = FRotator::ZeroRotator;
			LocalRotation.Yaw = AimingRotation.Yaw;
			TurnInPlace(LocalRotation,1.0f,0.0f,false);
		}
	}
	else
	{
		ElapsedDelayTime = 0.0f;
	}
}

void UAnimInstance_Character::RotateInPlaceCheck()
{
	// 步骤1:通过检查瞄准角度是否超过阈值来检查角色是否应该向左或向右旋转。
	Rotate_L = AimingAngle.X < RotateMinThreshold;
	Rotate_R = AimingAngle.X > RotateMaxThreshold;

	// 步骤2：如果角色应该旋转，请将“旋转速率”设置为与“目标偏航速率”一起缩放。这使角色在更快地移动摄影机时旋转得更快。
	if (Rotate_L || Rotate_R)
	{
		RotateRate = FMath::GetMappedRangeValueClamped(FVector2D(AimYawRateMinRange, AimYawRateMaxRange), FVector2D(MinPlayRate, MaxPlayRate), AimYawRate);
	}
}

void UAnimInstance_Character::DynamicTransitionCheck()
{
	auto LocalComponent = GetOwningComponent();
	float Distance = GetDistanceBetweenTwoSocketsAndMapRange(LocalComponent,FName("ik_foot_l"),RTS_Component,FName("VB foot_target_l"),RTS_Component);
	if (Distance > 8.0f)
	{
		// DisplayDebug()
	}

	Distance = GetDistanceBetweenTwoSocketsAndMapRange(LocalComponent,FName("ik_foot_r"),RTS_Component,FName("VB foot_target_r"),RTS_Component);
	if (Distance > 8.0f)
	{
		// DisplayDebug()
	}

}

// 计算速度混合。这个值表示参与者在每个方向上的速度量(标准化使每个方向的对角线等于.5)，并在BlendMulti节点中使用，以产生比标准blendspace更好的方向混合。
FVelocityBlend_ZMJ UAnimInstance_Character::CalculateVelocityBlend()
{
	// 速度是世界的，玩家朝向也是世界的，现在需要基于玩家坐标的方向
	auto LocRelativeVelocityDir = Character->GetActorRotation().UnrotateVector(Velocity.GetSafeNormal(0.1f));
	float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) + FMath::Abs(LocRelativeVelocityDir.Z);
	check(Sum > 0);
	auto RelavtiveDirection = LocRelativeVelocityDir / Sum;
	FVelocityBlend_ZMJ LocalVelocityBlend;
	LocalVelocityBlend.F = FMath::Clamp(RelavtiveDirection.X,0.0f,1.0f);
	LocalVelocityBlend.B = FMath::Abs(FMath::Clamp(RelavtiveDirection.X,-1.0f,0.0f));
	LocalVelocityBlend.L = FMath::Abs(FMath::Clamp(RelavtiveDirection.Y,-1.0f,0.0f));
	LocalVelocityBlend.R = FMath::Clamp(RelavtiveDirection.Y,0.0f,1.0f);

	// auto Location = Character->GetActorLocation();
	// UE_VLOG_ARROW(this, "AnimInstace", Verbose,Location,Location + RelavtiveDirection,FColor::Red,TEXT("AnimInstace CalculateVelocityBlend"));

	UE_VLOG(this, "AnimInstace", Verbose,
	TEXT("Name (%s)\n key F (%f)\n key B (%f)\n key L (%f)\n key R (%f)\n"),
	*GetName(),
	LocalVelocityBlend.F,
	LocalVelocityBlend.B,
	LocalVelocityBlend.L,
	LocalVelocityBlend.R
	);
	
	return LocalVelocityBlend;
}

// 计算对角刻度量。这个值用于缩放Foot IK Root骨，以使Foot IK骨在对角线混合上覆盖更多的距离。如果没有结垢，由于IK骨的线性平移混合，脚将不能在对角线方向上移动足够远。曲线用于方便地映射值。
float UAnimInstance_Character::CalculateDiagonalScaleAmount()
{
	if (IsValid(DiagonalScaleAmountCurve))
	{
		return DiagonalScaleAmountCurve->GetFloatValue( FMath::Abs(VelocityBlend.F + VelocityBlend.B));
	}
	return 0.0f;
}

// 计算相对加速度量。这个值表示相对于actor旋转的当前加速/减速量。它被归一化为-1到1的范围，因此-1等于最大制动减速，1等于角色移动组件的最大加速度。
FVector UAnimInstance_Character::CalculateRelativeAccelerationAmount()
{
	float LocalMaxAcceleration;
	if (FVector::DotProduct(Acceleration, Velocity) > 0.0f)
	{
		// 加速
		LocalMaxAcceleration = Character->GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{
		// 减速
		LocalMaxAcceleration = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
	}
	auto LocalAcceleration = Acceleration.GetClampedToMaxSize(LocalMaxAcceleration) / LocalMaxAcceleration;
	return Character->GetActorRotation().UnrotateVector(LocalAcceleration);
}

float UAnimInstance_Character::CalculateWalkRunBlend()
{
	return ActualGait == EGait_ZMJ::Walking ? 0.0f : 1.0;
}

// 计算步幅混合。这个值在混合空间中用于缩放步幅(脚移动的距离)，以便角色可以以不同的移动速度行走或奔跑。它还允许行走或奔跑的步态动画独立混合，同时仍然匹配动画速度与移动速度，防止角色需要发挥半走+半跑混合。曲线用于映射步幅量到最大控制速度。
float UAnimInstance_Character::CalculateStrideBlend()
{
	auto A = GetFloatValue(StrideBlend_N_Walk,Speed);
	auto B = GetFloatValue(StrideBlend_N_Run,Speed);
	auto Alpha = GetAnimCurve_Clamped(FName("Weight_Gait"), -1.0f, 0.0f, 1.0f);

	// 考虑 步伐 还有 蹲站
	return FMath::Lerp(FMath::Lerp(A, B, Alpha), GetFloatValue(StrideBlend_C_Walk,Speed), GetCurveValue(FName("BasePose_CLF")));
}

float UAnimInstance_Character::CalculateStandingPlayRate()
{
	auto localRelativeSpeed = FMath::Lerp(Speed / AnimatedWalkSpeed,Speed / AnimatedRunSpeed,GetAnimCurve_Clamped(FName("Weight_Gait"),-1.0,0.0f,1.0f));
	localRelativeSpeed = FMath::Lerp(localRelativeSpeed,Speed / AnimatedSprintSpeed,GetAnimCurve_Clamped(FName("Weight_Gait"),-2.0,0.0f,1.0f));

	return FMath::Clamp((localRelativeSpeed / StrdeBlend / GetOwningComponent()->GetComponentScale().Z),0.0f,3.0f);
}

// 通过跟踪速度方向来计算着陆预测权重，找到角色坠落的可行走表面，并获得“时间”(范围为0-1,1为最大值，0为即将着陆)直到撞击。土地预测曲线用于控制时间如何影响平滑混合的最终权重。
float UAnimInstance_Character::CalculateCrouchingPlayRate()
{
	return FMath::Clamp((Speed / AnimatedCrouchSpeed / StrdeBlend/ GetOwningComponent()->GetComponentScale().Z),0.0f,2.0f);
}

// 通过跟踪速度方向来计算着陆预测权重，找到角色坠落的可行走表面，并获得“时间”(范围为0-1,1为最大值，0为即将着陆)直到撞击。土地预测曲线用于控制时间如何影响平滑混合的最终权重。
float UAnimInstance_Character::CalculateLandPrediction()
{
	if (FallSpeed < -200.0f)
	{
		auto LocalCapsuleComponent = Character->GetCapsuleComponent();
		auto LocalStart = LocalCapsuleComponent->GetComponentLocation();
		auto LocalEndOffset = FVector::ZeroVector;
		LocalEndOffset.X = Velocity.X;
		LocalEndOffset.Y = Velocity.Y;
		LocalEndOffset.Z = FMath::Clamp(Velocity.Z, -4000.0f, -200.0f);
		LocalEndOffset = LocalEndOffset.GetUnsafeNormal() * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, -4000.0f), FVector2D(50.0f, 2000.0f), Velocity.Z);;
		auto LocalEnd = LocalStart + LocalEndOffset;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(Character);
		FHitResult OutHit;
		UKismetSystemLibrary::CapsuleTraceSingleByProfile(GetWorld(),LocalStart,LocalEnd,LocalCapsuleComponent->GetUnscaledCapsuleRadius(),LocalCapsuleComponent->GetUnscaledCapsuleHalfHeight(),
			FName("Pawn"),false,IgnoreActors,GetTraceDebugType(EDrawDebugTrace::ForOneFrame),OutHit,true,FColor::Red,FColor::Green,5.0f);
		bool bLocalWalkable = Cast<UCharacterMovementComponent>(Character->GetMovementComponent())->IsWalkable(OutHit);
		bool bLocalBlockingHit = OutHit.bBlockingHit;
		bool bTest = OutHit.IsValidBlockingHit();
		if (bLocalWalkable && bLocalBlockingHit && bTest)
		{
			return FMath::Lerp(LandPredictionCurve->GetFloatValue(OutHit.Time),0.0f,GetCurveValue(FName("Mask_LandPrediction")));
		}
		else
		{
			return 0.0f;
		}
	}
	else
	{
		return 0.0f;
	}
}

// 使用相对速度方向和数量来决定角色在空中应该倾斜多少。在空中倾斜曲线获得下降速度，并被用作乘数，在从向上移动到向下移动的过渡中平稳地逆转倾斜方向。
FLeanAmount_ZMJ UAnimInstance_Character::CalculateInAirLeanAmount()
{
	auto LocalVector = Character->GetActorRotation().UnrotateVector(Velocity) / 350.0f;
	FLeanAmount_ZMJ OutResult;
	OutResult.LR = LocalVector.Y * LeanInAirCurve->GetFloatValue(FallSpeed);
	OutResult.FB = LocalVector.X * LeanInAirCurve->GetFloatValue(FallSpeed);
	return OutResult;
}

void UAnimInstance_Character::SetFootOffsets(FName Enable_FootIK_Curve, FName IKFootBone, FName RootBone,
	FVector& CurrentLocationTarget, FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset)
{
	if( GetCurveValue(Enable_FootIK_Curve) > 0 )
	{
		// 步骤1：从脚位置向下追踪以找到几何图形。如果曲面是可行走的，请保存“碰撞位置”和“法线”。
		FVector IKFootFloorLocation;
		IKFootFloorLocation.X = GetOwningComponent()->GetSocketLocation(IKFootBone).X;
		IKFootFloorLocation.Y = GetOwningComponent()->GetSocketLocation(IKFootBone).Y;
		IKFootFloorLocation.Z = GetOwningComponent()->GetSocketLocation(RootBone).Z;

		auto StartLocation = IKFootFloorLocation;
		StartLocation.Z += IK_TraceDistanceAboveFoot;
		auto EndLocation = IKFootFloorLocation;
		EndLocation.Z -= IK_TraceDistanceBelowFoot;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(Character);
		FHitResult OutHit;

		UKismetSystemLibrary::LineTraceSingle(GetWorld(),StartLocation,EndLocation,ETraceTypeQuery::TraceTypeQuery1,false,IgnoreActors,GetTraceDebugType(EDrawDebugTrace::ForOneFrame),
		OutHit,true,FColor::Red,FColor::Green,5.0f);

		FVector ImpactPoint;
		FVector ImpactNormal;
		FRotator TargetRotationOffset;
		
		if (Character->GetCharacterMovement()->IsWalkable(OutHit))
		{
			ImpactPoint = OutHit.ImpactPoint;
			ImpactNormal = OutHit.ImpactNormal;

			// 步骤1.1：找出撞击点与预期（平坦）地板位置之间的位置差异。这些值通过标称值乘以脚高度进行偏移，以在倾斜曲面上获得更好的行为。
			CurrentLocationTarget = ImpactNormal * FootHeight + ImpactPoint - FVector(IKFootFloorLocation.X,IKFootFloorLocation.Y,IKFootFloorLocation.Z + FootHeight);
			// 步骤1.2：通过获取碰撞法线的Atan2来计算旋转偏移。
			TargetRotationOffset.Roll = (180.f)/PI * FMath::Atan2(ImpactNormal.Y, ImpactNormal.Z);
			TargetRotationOffset.Pitch = (180.f)/PI * FMath::Atan2(ImpactNormal.X, ImpactNormal.Z) * -1;
			TargetRotationOffset.Yaw = 0;
		}


		// 步骤2：将当前位置偏移插入到新的目标值。根据新目标是高于还是低于当前目标，以不同的速度进行插值。
		if (CurrentLocationOffset.Z > CurrentLocationTarget.Z)
		{
			CurrentLocationOffset = FMath::VInterpTo(CurrentLocationOffset,CurrentLocationTarget,MyDeltaTimeX,30.0f);
		}
		else
		{
			CurrentLocationOffset = FMath::VInterpTo(CurrentLocationOffset,CurrentLocationTarget,MyDeltaTimeX,15.0f);
		}

		// 步骤3：将“当前旋转偏移”插值到新的目标值。
		CurrentRotationOffset = FMath::RInterpTo(CurrentRotationOffset,TargetRotationOffset,MyDeltaTimeX,30.0f);
	}
	else
	{
		CurrentLocationOffset = FVector::ZeroVector;
		CurrentRotationOffset = FRotator::ZeroRotator;
	}
}

void UAnimInstance_Character::SetPelvisIKOffset(FVector FootOffset_L_Target, FVector FootOffset_R_Target)
{
	// 通过找到脚IK的平均权重来计算骨盆Alpha。如果alpha为0，请清除偏移。
	PelvisAlpha = (GetCurveValue(FName("Enable_FootIK_L")) + GetCurveValue(FName("Enable_FootIK_R"))) / 2;
	FVector PelvisTarget;
	if (PelvisAlpha > 0.0f)
	{
		// 步骤1：将新的骨盆目标设置为最低的脚部偏移
		if (FootOffset_L_Target.Z < FootOffset_R_Target.Z)
		{
			PelvisTarget = FootOffset_L_Target;
		}
		else
		{
			PelvisTarget = FootOffset_R_Target;
		}

		// 步骤2：将“当前骨盆偏移”插值到新的目标值。根据新目标是高于还是低于当前目标，以不同的速度进行插值。
		if (PelvisTarget.Z > PelvisOffset.Z)
		{
			PelvisOffset = FMath::VInterpTo(PelvisOffset,PelvisTarget,MyDeltaTimeX,10.0f);
		}
		else
		{
			PelvisOffset = FMath::VInterpTo(PelvisOffset,PelvisTarget,MyDeltaTimeX,15.0f);
		}
	}
	else
	{
		PelvisOffset = FVector::ZeroVector;
	}
}

void UAnimInstance_Character::SetFootLocking(FName Enable_FootIK_Curve, FName FootLockCurve, FName IKFootBone,
                                             float& CurrentFootLockAlpha, FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation)
{
	// 只有曲线 Enable_FootIK_Curve 的值大于0才能有脚部IK
	if( GetCurveValue(Enable_FootIK_Curve) > 0 )
	{
		// 步骤1：设置“局部锁定曲线”值
		float FootLockCurveValue = GetCurveValue(FootLockCurve);

		// 步骤2：仅当新值小于当前值或等于1时更新FootLock Alpha。这使得脚只能从锁定位置或锁定到新位置，而不能融入。
		if(FootLockCurveValue >= 0.99 || FootLockCurveValue < CurrentFootLockAlpha)
		{
			CurrentFootLockAlpha = FootLockCurveValue;
		}

		// 步骤3：如果“脚锁定”曲线等于1，请在零部件空间中保存新的锁定位置和旋转。
		if (CurrentFootLockAlpha >= 0.99)
		{
			auto LocalTransform = GetOwningComponent()->GetSocketTransform(IKFootBone,RTS_Component);
			CurrentFootLockLocation = LocalTransform.GetLocation();
			CurrentFootLockRotation = LocalTransform.Rotator();
		}

		// 步骤4：如果“脚锁定Alpha”具有权重，请更新“脚锁定”偏移，以在胶囊移动时将脚固定到位。
		if (CurrentFootLockAlpha > 0.0)
		{
			SetFootLockOffsets(CurrentFootLockLocation,CurrentFootLockRotation);
		}
	}
}

void UAnimInstance_Character::SetFootLockOffsets(FVector& LocalLocation, FRotator& LocalRotation)
{
	// 使用当前和上次更新的旋转之间的差值来计算脚应该旋转多少才能保持在地面上
	auto RotationDifference = FRotator::ZeroRotator;
	if (Character->GetCharacterMovement()->IsMovingOnGround())
	{
		RotationDifference = Character->GetActorRotation() - Character->GetCharacterMovement()->GetLastUpdateRotation();
		RotationDifference.Normalize();
	}

	// 获取帧之间相对于网格旋转的距离，以确定脚在地面上保持踩踏时应偏移的距离。
	auto LocationDifference = GetOwningComponent()->GetComponentRotation().UnrotateVector(Velocity * GetWorld()->GetDeltaSeconds());

	// 从当前局部位置减去位置差，然后将其旋转旋转差，以使脚保持在构件空间中。
	auto Local =  LocalLocation - LocationDifference;
	LocalLocation = Local.RotateAngleAxis(RotationDifference.Yaw, FVector(0.0f,0.0f,-1.0f));

	// 从当前“局部旋转”中减去“旋转差”以获得新的局部旋转。
	LocalRotation = LocalRotation - RotationDifference;
	LocalRotation.Normalize();
}

void UAnimInstance_Character::ResetIKOffsets()
{
	// 插值脚IK偏移回0
	FootOffset_L_Location = FMath::VInterpTo(FootOffset_L_Location,FVector::ZeroVector,MyDeltaTimeX,15.0f);
	FootOffset_R_Location = FMath::VInterpTo(FootOffset_R_Location,FVector::ZeroVector,MyDeltaTimeX,15.0f);
	
	FootOffset_L_Rotation = FMath::RInterpTo(FootOffset_L_Rotation,FRotator::ZeroRotator,MyDeltaTimeX,15.0f);
	FootOffset_R_Rotation = FMath::RInterpTo(FootOffset_L_Rotation,FRotator::ZeroRotator,MyDeltaTimeX,15.0f);
}

// 计算运动方向。这个值表示角色在看方向/瞄准旋转模式中相对于相机移动的方向，并在循环混合动画图层中使用，以混合到适当的方向状态。
EMovementDirection_ZMJ UAnimInstance_Character::CalculateMovementDirection()
{
	if (ActualGait == EGait_ZMJ::Sprinting)
	{
		return EMovementDirection_ZMJ::Forward;
	}
	else
	{
		if (RotationMode == ERotationMode_ZMJ::VelocityDirection)
		{
			return EMovementDirection_ZMJ::Forward;;
		}
		else
		{
			auto LocalRotation = Velocity.ToOrientationRotator() - AimingRotation;
			LocalRotation.Normalize();
			
			UE_VLOG(this, "AnimInstace", Verbose,
			TEXT("Name (%s)/n key RelativeYaw (%f)"),
			*GetName(),
			LocalRotation.Yaw
			);
			
			return CalculateQuadrant(MovementDirection,70,-70,110,-110,5,LocalRotation.Yaw);
		}
	}
}

// 取输入角并确定其象限(方向)。使用当前移动方向来增加或减少每个象限的角度范围上的缓冲。
EMovementDirection_ZMJ UAnimInstance_Character::CalculateQuadrant(EMovementDirection_ZMJ Current, float FR_Threshold,
	float FL_Threshold, float BR_Threshold, float BL_Threshold, float Buffer, float Angel)
{
	if (AngleInRange(Angel,FL_Threshold,FR_Threshold,Buffer,Current != EMovementDirection_ZMJ::Forward || Current != EMovementDirection_ZMJ::Backward))
	{
		return EMovementDirection_ZMJ::Forward;
	}
	else if (AngleInRange(Angel,FR_Threshold,BR_Threshold,Buffer,Current != EMovementDirection_ZMJ::Right || Current != EMovementDirection_ZMJ::Left))
	{
		return EMovementDirection_ZMJ::Right;
	}
	else if (AngleInRange(Angel,BL_Threshold,FL_Threshold,Buffer,Current != EMovementDirection_ZMJ::Right || Current != EMovementDirection_ZMJ::Left))
	{
		return EMovementDirection_ZMJ::Left;
	}
	else
	{
		return EMovementDirection_ZMJ::Backward;
	}
}

bool UAnimInstance_Character::AngleInRange(float Angle, float MinAngel, float MaxAngel, float Buffer,
	bool IncreaseBuffer)
{
	float MaxValue; 
	float MinValue; 
	if (IncreaseBuffer)
	{
		MaxValue = MaxAngel + Buffer;
		MinValue = MinAngel - Buffer;
	}
	else
	{
		MaxValue = MaxAngel - Buffer;
		MinValue = MinAngel + Buffer;
	}
	return (Angle >= MinAngel) && (Angle <= MaxAngel);
}

FVelocityBlend_ZMJ UAnimInstance_Character::InterpVelocityBlend(FVelocityBlend_ZMJ Current, FVelocityBlend_ZMJ Target,
                                                                float InterpSpeed, float DeltaTime)
{
	FVelocityBlend_ZMJ Result;
	Result.F = FMath::FInterpTo(Current.F,Target.F,DeltaTime,InterpSpeed);
	Result.B = FMath::FInterpTo(Current.B,Target.B,DeltaTime,InterpSpeed);
	Result.L = FMath::FInterpTo(Current.L,Target.L,DeltaTime,InterpSpeed);
	Result.R = FMath::FInterpTo(Current.R,Target.R,DeltaTime,InterpSpeed);
	return Result;
}

FLeanAmount_ZMJ UAnimInstance_Character::InterpLeanAmount(FLeanAmount_ZMJ Current, FLeanAmount_ZMJ Target,
	float InterpSpeed, float DeltaTime)
{
	FLeanAmount_ZMJ Result;
	Result.LR = FMath::FInterpTo(Current.LR,Target.LR,DeltaTime,InterpSpeed);
	Result.FB = FMath::FInterpTo(Current.FB,Target.FB,DeltaTime,InterpSpeed);
	return Result;
}

EDrawDebugTrace::Type UAnimInstance_Character::GetTraceDebugType(EDrawDebugTrace::Type ShowTraceType)
{
	// TODO 这里是判断是不是要调试绘出图形 现在先默认返回调试
	return ShowTraceType;
}

void UAnimInstance_Character::Jumped()
{
	bJumped = true;
	JumpPlayRate = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 600.0f), FVector2D(1.2, 1.5), Speed);
	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle,[this]
	{
		bJumped = false;
	},0.1f,false);
}

float UAnimInstance_Character::GetAnimCurve_Clamped(FName Name, float Bias, float ClmapMin, float ClampMax)
{
	return  FMath::Clamp(GetCurveValue(Name) + Bias, ClmapMin, ClampMax);
}

float UAnimInstance_Character::GetFloatValue(UCurveFloat* CurveFloat, float InTime)
{
	if (IsValid(CurveFloat))
	{
		return CurveFloat->GetFloatValue(InTime);
	}
	return 0.0f;
}

FVector UAnimInstance_Character::GetVectorValue(UCurveVector* CurveVector, float InTime)
{

	if (IsValid(CurveVector))
	{
		return CurveVector->GetVectorValue(InTime);
	}
	return FVector::ZeroVector;
}

float UAnimInstance_Character::GetDistanceBetweenTwoSocketsAndMapRange(const USkeletalMeshComponent* Component, const FName SocketOrBoneNameA, ERelativeTransformSpace SocketSpaceA, const FName SocketOrBoneNameB, ERelativeTransformSpace SocketSpaceB, bool bRemapRange, float InRangeMin, float InRangeMax, float OutRangeMin, float OutRangeMax)
{
	if (Component && SocketOrBoneNameA != NAME_None && SocketOrBoneNameB != NAME_None)
	{
		FTransform SocketTransformA = Component->GetSocketTransform(SocketOrBoneNameA, SocketSpaceA);
		FTransform SocketTransformB = Component->GetSocketTransform(SocketOrBoneNameB, SocketSpaceB);

		float Distance = (SocketTransformB.GetLocation() - SocketTransformA.GetLocation()).Size();

		if (bRemapRange)
		{
			return FMath::GetMappedRangeValueClamped(FVector2D(InRangeMin, InRangeMax), FVector2D(OutRangeMin, OutRangeMax), Distance);
		}
		else
		{
			return Distance;
		}
	}

	return 0.f;
}
