// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance_Character.h"
#include "AnimationDemo/Character/CharacterInformationInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	switch (MovementState)
	{
	case EMovementState_ZMJ::Grounded:
		auto LastShouldMove = ShouldMove;
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
			// 原地旋转功能
			// if (CanRotateInPlace())
			// {
			// 	RotateInPlaceCheck();
			// }
			// else
			// {
			// 	Rotate_L = false;
			// 	Rotate_R = false;
			// }

			// 
				
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
	}

	UE_VLOG(this, "AnimInstace", Verbose,
	TEXT("Name (%s)/n key StrdeBlend (%f)/n key WalkRunBlend (%f)/n"),
	*GetName(),
	StrdeBlend,
	WalkRunBlend
	);
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
	/*// Interp瞄准旋转值以实现平滑的瞄准旋转变化。在计算角度之前插入旋转确保该值不受actor旋转变化的影响，允许缓慢的瞄准旋转变化与快速的actor旋转变化。
	SmoothedAimingRotation = FMath::RInterpTo(SmoothedAimingRotation,AimingRotation,MyDeltaTimeX,SmoothedAimingRotationInterpSpeed);

	auto LocalRotation = AimingRotation - Character->GetActorRotation();
	LocalRotation.Normalize();
	AimingAngle.X = LocalRotation.Yaw;
	AimingAngle.Y = LocalRotation.Pitch;

	// 通过计算目标旋转与动作者旋转之间的差值，计算目标角和平滑目标角。
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
	ForwardYawTime = FMath::GetMappedRangeValueClamped(FVector2D(-180.0f, 180.0f), FVector2D(0.0f, 1.0f), SmoothedAimingAngle.Y);
	*/

}

void UAnimInstance_Character::UpdateLayerValues()
{
}

void UAnimInstance_Character::UpdateFootIK()
{
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

bool UAnimInstance_Character::ShouldMoveCheck()
{
	return Speed > 150 || (IsMoving && HasMovementInput);
}

// 仅当角色以第三人称朝向摄影机并且“启用过渡”曲线已完全加权时，才执行“原地转弯”检查。Enable_Transition曲线在AnimBP的某些状态下进行修改，以便角色只能在这些状态下转动。
bool UAnimInstance_Character::CanTurnInPalce()
{
	return RotationMode == ERotationMode_ZMJ::LookingDirection && ViewMode == EViewMode_ZMJ::ThirdPerson && GetCurveValue(FName("Enable_Transition")) > 0.99;
}

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
	// Step 1: Check if the character should rotate left or right by checking if the Aiming Angle exceeds the threshold.
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

	// TODO 这里之后要加入冲刺相关的
	
	return FMath::Clamp((localRelativeSpeed / StrdeBlend / GetOwningComponent()->GetComponentScale().X),0.0f,3.0f);
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
