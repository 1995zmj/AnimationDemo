// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_ZMJ.h"

#include "DrawDebugHelpers.h"
#include "K2Node_GetDataTableRow.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VisualLogger/VisualLogger.h"

// Sets default values
ACharacter_ZMJ::ACharacter_ZMJ()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 需要确定的预置
	DesiredGait = EGait_ZMJ::Runing;
	DesiredRotationMode = ERotationMode_ZMJ::LookingDirection;
	ViewMode = EViewMode_ZMJ::ThirdPerson;
	OverlayState =EOverlayState_ZMJ::Default;
	DesiredStance = EStance_ZMJ::Standing;

	LookUpRate = 1.25;
	LookRightRate = 1.25;

	PreviousVelocity = FVector::ZeroVector;
	MovementModeChangedDelegate.AddDynamic(this, &ACharacter_ZMJ::OnMovementModeChangedDDelegateEvent);
}

// Called when the game starts or when spawned
void ACharacter_ZMJ::BeginPlay()
{ 
	Super::BeginPlay();
	OnBeginPlay();
	
}

// Called every frame
void ACharacter_ZMJ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetEssentialValues();
	switch (MovementState)
	{
	case EMovementState_ZMJ::Grounded:
		UpdateCharacterMovement();
		UpdateGroudedRotation();
		break;
	}
	// FCharacterInformation_ZMJ OutCharacterInformation;
	// GetEssentialValues(OutCharacterInformation);
	// UE_VLOG(this, "ACharacter_ZMJ", Verbose,
	// TEXT("Name (%s)/n key Velocity (%s)/n key Speed (%f)/n"),
	// *GetName(),
	// *OutCharacterInformation.Velocity.ToString(),
	// OutCharacterInformation.Speed
	// );

}

// Called to bind functionality to input
void ACharacter_ZMJ::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward/Backwards", this, &ACharacter_ZMJ::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight/Left", this, &ACharacter_ZMJ::MoveRight);

	PlayerInputComponent->BindAxis("LookLeft/Right", this, &ACharacter_ZMJ::LookRight);
	PlayerInputComponent->BindAxis("LookUp/Down", this, &ACharacter_ZMJ::LookUp);
}


void ACharacter_ZMJ::MoveForward(float Value)
{
	PlayerMovementInput(true, Value);
}

void ACharacter_ZMJ::MoveRight(float Value)
{
	PlayerMovementInput(false, Value);
}

void ACharacter_ZMJ::LookUp(float Value)
{
	AddControllerPitchInput(Value * LookUpRate);
}

void ACharacter_ZMJ::LookRight(float Value)
{
	AddControllerYawInput(Value * LookRightRate);
}

FVector ACharacter_ZMJ::GetControlForwardVector()
{
	auto LocalRotation = FRotator::ZeroRotator;
	LocalRotation.Yaw = GetControlRotation().Yaw;
	return LocalRotation.Vector();
}

FVector ACharacter_ZMJ::GetControlRightVector()
{
	auto LocalRotation = FRotator::ZeroRotator;
	LocalRotation.Yaw = GetControlRotation().Yaw;
	return FRotationMatrix(LocalRotation).GetScaledAxis(EAxis::Y);
}

float ACharacter_ZMJ::GetAnimCurveValue(FName CurveName)
{
	if (IsValid(MainAnimInstance))
	{
		return MainAnimInstance->GetCurveValue(CurveName);
	}

	return  0.0f;
}

void ACharacter_ZMJ::PlayerMovementInput(bool IsForwardAxis, float AxisValue)
{
	switch (MovementState)
	{
	case EMovementState_ZMJ::Grounded:
		// 默认的相机相对运动行为
		if (IsForwardAxis)
		{
			AddMovementInput(GetControlForwardVector(),FixDiagonalGamepadValues(AxisValue));
		}
		else
		{
			auto LocalWorldDirection = GetControlRightVector();
			auto ScaleValue = FixDiagonalGamepadValues(AxisValue);
			AddMovementInput(LocalWorldDirection,ScaleValue);
		}
		break;
	}
}

// 修复对角线手柄值
float ACharacter_ZMJ::FixDiagonalGamepadValues(float AxisValue)
{
	auto t = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 0.6f), FVector2D(1.0f, 1.2f), FMath::Abs(AxisValue));
	return FMath::Clamp(t * AxisValue,-1.0f,1.0f);
}

// 这些值表示胶囊的移动方式以及它想要移动的方式，因此对于任何数据驱动的动画系统都是必不可少的。它们也在整个系统中用于各种功能，所以我发现在一个地方管理它们是最容易的。
void ACharacter_ZMJ::SetEssentialValues()
{
	Acceleration = CalculateAcceleration();
	// UE_LOG(LogTemp, Warning, TEXT("Acceleration : %f"),Acceleration.Size());

	// UE_LOG(LogTemp, Warning, TEXT("ue Acceleration : %f"),GetCharacterMovement()->GetCurrentAcceleration().Size());
	//通过获取角色的速度来确定角色是否在移动。速度等于水平(x，y)速度的长度，因此不考虑垂直移动。如果角色正在移动，请更新上次的速度旋转。保存该值是因为即使在角色停止之后，了解移动的最后方向可能也很有用。
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();
	// UE_LOG(LogTemp, Warning, TEXT("speed : %f"),Speed);

	// 有速度就是在移动
	IsMoving = Speed > 1.0f;
	if (IsMoving)
	{
		// 返回向量指向的方向对应的FRotator方向。
		// 将偏航和俯仰设置为正确的数字，并将滚动设置为零，因为无法根据矢量确定滚动。
		LastVelocityRotation = GetVelocity().ToOrientationRotator();
	}
	
	// 通过获取角色的移动输入量来确定角色是否有移动输入。移动输入量等于当前加速度除以最大加速度，因此它的范围是0-1，1是最大可能的输入量，0表示没有。如果角色有移动输入，则更新上次移动输入旋转。
	auto CharacterMovementRef = GetCharacterMovement();
	auto TempAcceleration = CharacterMovementRef->GetCurrentAcceleration();
	float AccelerationLength = TempAcceleration.Size();
	float MaxAcceleration = CharacterMovementRef->GetMaxAcceleration();
	MovementInputAmount = AccelerationLength/MaxAcceleration;
	// 这里指的是输入量，没有输入的时候会缓慢停止，有个过渡的，还是在移动的
	HasMovementInput = MovementInputAmount > 0.0;
	if (HasMovementInput)
	{
		LastMovementInputRotation = TempAcceleration.ToOrientationRotator();
	}

	// 通过比较当前和上一个目标偏航值除以增量秒来设置目标偏航率。这表示摄影机从左向右旋转的速度。
	AimYawRate = FMath::Abs((GetControlRotation().Yaw - PreviousAimYaw) / GetWorld()->GetDeltaSeconds());
}

// 缓存要在下一帧计算中使用的某些值
void ACharacter_ZMJ::CacheValues()
{
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = GetControlRotation().Yaw;
}

// 通过比较当前速度和以前的速度来计算加速度。移动组件返回的当前加速度等于输入加速度，并不代表角色的实际物理加速度。
FVector ACharacter_ZMJ::CalculateAcceleration()
{
	
	return (GetVelocity() - PreviousVelocity) / GetWorld()->GetDeltaSeconds();
}

void ACharacter_ZMJ::OnBeginPlay()
{
	// 确保 actor 的参数更新完，在更新动画蓝图的
	auto TempMesh = GetMesh();
	TempMesh->AddTickPrerequisiteActor(this);
	
	// 设置参考主animm实例。
	MainAnimInstance = TempMesh->GetAnimInstance();

	SetMovementModel();

	//更新状态以使用所需的初始值。
	OnGaitChanged(DesiredGait);
	OnRotationModeChanged(DesiredRotationMode);
	OnViewModeChanged(ViewMode);
	OnOverlayStateChanged(OverlayState);

	switch (DesiredStance)
	{
	case EStance_ZMJ::Standing:
		UnCrouch();
		break;
	case EStance_ZMJ::Crouching:
		Crouch();
		break; 
	}

	//设置默认的旋转值。
	TargetRotation = GetActorRotation();
	LastVelocityRotation = TargetRotation;
	LastMovementInputRotation = TargetRotation;
	
}

void ACharacter_ZMJ::OnMovementModeChangedDDelegateEvent(ACharacter* Character, EMovementMode PrevMovementMode,
	uint8 PreviousCustomMode)
{
	OnCharacterMovementModeChanged(PrevMovementMode,GetCharacterMovement()->MovementMode,PrevMovementMode,GetCharacterMovement()->CustomMovementMode);
}

// 使用角色移动模式更改设置移动状态为正确的值。这允许你拥有一组自定义的移动状态，但仍然使用默认角色移动组件的功能。
void ACharacter_ZMJ::OnCharacterMovementModeChanged(EMovementMode PrevMovementMode, EMovementMode NewMovementMode,
	uint8 PrevCustomMode, uint8 NewCustomMode)
{
	switch (NewMovementMode)
	{
	case  EMovementMode::MOVE_Walking:
	case  EMovementMode::MOVE_NavWalking:
		SetMovementState(EMovementState_ZMJ::Grounded);
		break;
	}
}

void ACharacter_ZMJ::OnMovementStateChanged(EMovementState_ZMJ NewMovementState)
{
	MovementState = NewMovementState;
	// TODO 还有其他设置等下次
	// switch (MovementState) {  }
}

void ACharacter_ZMJ::OnGaitChanged(EGait_ZMJ NewActualGait)
{
	Gait = NewActualGait;
}

void ACharacter_ZMJ::OnRotationModeChanged(ERotationMode_ZMJ NewRotationMode)
{
	RotationMode = NewRotationMode;
	if (RotationMode == ERotationMode_ZMJ::VelocityDirection)
	{
		if (ViewMode == EViewMode_ZMJ::FirstPerson)
		{
			SetViewMode(EViewMode_ZMJ::ThirdPerson);
		}
	}
}

void ACharacter_ZMJ::OnViewModeChanged(EViewMode_ZMJ NewViewMode)
{
	ViewMode = NewViewMode;
	switch (ViewMode)
	{
	case EViewMode_ZMJ::ThirdPerson:
		switch (RotationMode)
		{
		case ERotationMode_ZMJ::VelocityDirection:
			SetRotationMode(DesiredRotationMode);
			break;
		case ERotationMode_ZMJ::LookingDirection:
			SetRotationMode(DesiredRotationMode);
			break;
		case ERotationMode_ZMJ::Aiming:
			break;
		default:
			break;
		}
		break;
	case EViewMode_ZMJ::FirstPerson:
		switch (RotationMode)
		{
		case ERotationMode_ZMJ::VelocityDirection:
			SetRotationMode(ERotationMode_ZMJ::LookingDirection);
			break;
		case ERotationMode_ZMJ::LookingDirection:
			break;
		case ERotationMode_ZMJ::Aiming:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void ACharacter_ZMJ::OnOverlayStateChanged(EOverlayState_ZMJ NewOverlayState)
{
	OverlayState = NewOverlayState;
}

void ACharacter_ZMJ::SetMovementModel()
{
	FString ContextString;
	MovementData = MovementModel.GetRow<FMovementSettings_State_ZMJ>(ContextString);
}

void ACharacter_ZMJ::UpdateCharacterMovement()
{
	// 设置允许的步态
	auto AllowedGait = GetAllowedGait();

	// 确定实际的步态。如果与当前步态不同，则设置新的步态事件。
	auto ActualGait = GetActualGait(AllowedGait);
	if (ActualGait != Gait)
	{
		SetGait(ActualGait);
	}
	
	// 使用允许的步态来更新运动设置。
	UpdateDynamicMovementSetting(AllowedGait);
}

void ACharacter_ZMJ::UpdateDynamicMovementSetting(EGait_ZMJ AllowedGait)
{
	CurrentMovementSettings = GetTargetMovementSettings();
	auto TempCharacterMovement = GetCharacterMovement();

	auto localSpeed = 0.0f;
	// 根据当前允许的步态，更新角色最大行走速度到配置的速度。
	switch (AllowedGait)
	{
	case EGait_ZMJ::Walking:
		localSpeed = CurrentMovementSettings.WalkSpeed;
		break;
	case EGait_ZMJ::Runing:
		localSpeed = CurrentMovementSettings.RunSpeed;
		break;
	case EGait_ZMJ::Sprinting:
		localSpeed = CurrentMovementSettings.SprintSpeed;
		break;
	}
	// UE_LOG(LogTemp, Warning, TEXT("current Gait : %s"),*StaticEnum<EGait_ZMJ>()->GetNameStringByValue((int8)AllowedGait));

	TempCharacterMovement->MaxWalkSpeed = localSpeed;
	TempCharacterMovement->MaxWalkSpeedCrouched = localSpeed;

	// 使用运动曲线更新加速度，减速和地面摩擦。这允许对每个速度的移动行为进行精细控制(可能不适合复制)。
	auto LocalValue = CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed());
	
	TempCharacterMovement->MaxAcceleration = LocalValue.X;
	TempCharacterMovement->BrakingDecelerationWalking = LocalValue.Y;
	TempCharacterMovement->GroundFriction = LocalValue.Z;
}

FMovementSettings_ZMJ ACharacter_ZMJ::GetTargetMovementSettings()
{
	auto localStance = Stance;
	auto Func = [localStance](FMovementSettings_Stance_ZMJ config)
	{
		switch (localStance)
		{
		case EStance_ZMJ::Standing:
			return config.Standing;
			break;
		case EStance_ZMJ::Crouching:
			return config.Crouching;
			break;
		}
		return config.Standing;
	};
	auto localMovementSettings = MovementData->VelocityDirection.Standing;
	switch (RotationMode)
	{
	case ERotationMode_ZMJ::VelocityDirection:
		localMovementSettings = Func(MovementData->VelocityDirection);
		break;
	case ERotationMode_ZMJ::LookingDirection:
		localMovementSettings = Func(MovementData->LookingDirection);
		break;
	case ERotationMode_ZMJ::Aiming:
		localMovementSettings = Func(MovementData->Aiming);
		break;
	}
	return localMovementSettings;
}

//计算允许的步态。这表示角色当前允许的最大步态，可以由所需步态、旋转模式、站姿等确定。例如，如果要在室内强制角色进入行走状态，可以在此处完成。
EGait_ZMJ ACharacter_ZMJ::GetAllowedGait()
{
	auto TempGait = EGait_ZMJ::Walking;
	switch (Stance)
	{
	case EStance_ZMJ::Standing:
		if (RotationMode == ERotationMode_ZMJ::Aiming)
		{
			TempGait = DesiredGait == EGait_ZMJ::Sprinting ? EGait_ZMJ::Runing : DesiredGait;
		}
		else
		{
			if (DesiredGait == EGait_ZMJ::Sprinting)
			{
				TempGait = CanSprint() ? DesiredGait : EGait_ZMJ::Runing;
			}
			else
			{
				TempGait = DesiredGait;
			}
		}
		break;
	case EStance_ZMJ::Crouching:
		TempGait = DesiredGait == EGait_ZMJ::Sprinting ? EGait_ZMJ::Runing : DesiredGait;
		break;
	}

	return TempGait;
}

//找出真正的步态。这是由角色的实际移动计算的，因此它可能不同于所需步态或允许的步态。例如，如果允许的步态变为行走，则实际步态仍将运行，直到角色减速到行走速度。
EGait_ZMJ ACharacter_ZMJ::GetActualGait(EGait_ZMJ AllowedGait)
{
	auto LocalWalkSpeed = CurrentMovementSettings.WalkSpeed;
	auto LocalRunSpeed = CurrentMovementSettings.RunSpeed;

	auto localGait = EGait_ZMJ::Walking;
	if (Speed >= LocalRunSpeed + 10.0f)
	{
		localGait = AllowedGait == EGait_ZMJ::Walking ? EGait_ZMJ::Runing : AllowedGait;
	}
	else if (Speed >= LocalWalkSpeed + 10.0f)
	{
		localGait = EGait_ZMJ::Runing;
	}
	
	return localGait;
}

//根据旋转模式和当前加速(输入)旋转来确定角色当前是否能够冲刺。如果角色处于look Rotation模式，那么只有当有完整的移动输入，并且与摄像机相对正或负50度时才允许冲刺。
bool ACharacter_ZMJ::CanSprint()
{
	if (HasMovementInput)
	{
		switch (RotationMode)
		{
		case ERotationMode_ZMJ::VelocityDirection:
			return MovementInputAmount > 0.9f;
			break;
		case ERotationMode_ZMJ::LookingDirection:
			auto LocalRotation = GetCharacterMovement()->GetCurrentAcceleration().ToOrientationRotator() - GetControlRotation();
			LocalRotation.Normalize();
			return (MovementInputAmount > 0.9f && FMath::Abs(LocalRotation.Yaw) < 50.f);
			break;	
		
		}
	}
	return false;
}

float ACharacter_ZMJ::GetMappedSpeed()
{
	auto localWalkSpeed = CurrentMovementSettings.WalkSpeed;
	auto localRunSpeed = CurrentMovementSettings.RunSpeed;
	auto localSprintSpeed = CurrentMovementSettings.SprintSpeed;

	if (Speed > localRunSpeed)
	{
		return 	FMath::GetMappedRangeValueClamped(FVector2D(localRunSpeed, localSprintSpeed), FVector2D(2.0f, 3.0f), Speed);
	}
	else if (Speed > localWalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(localWalkSpeed, localRunSpeed), FVector2D(1.0f, 2.0f), Speed);
	}
	
	return FMath::GetMappedRangeValueClamped(FVector2D(0.0f, localWalkSpeed), FVector2D(0.0f, 1.0f), Speed);
}

void ACharacter_ZMJ::UpdateGroudedRotation()
{
	switch (MovementAction)
	{
	case  EMovementAction_ZMJ::None:
		if (CanUpdateMovingRotation())
		{
			auto LocalRotator = FRotator::ZeroRotator;
			switch (RotationMode)
			{
			case ERotationMode_ZMJ::VelocityDirection:
				LocalRotator.Yaw = LastVelocityRotation.Yaw;
				SmoothCharacterRotation(LocalRotator,800,CalculateGroundedRotationRate());
				break;
			case ERotationMode_ZMJ::LookingDirection:
				if (Gait == EGait_ZMJ::Sprinting)
				{
					LocalRotator.Yaw = LastVelocityRotation.Yaw;
					SmoothCharacterRotation(LocalRotator,500,CalculateGroundedRotationRate());
				}
				else
				{
					LocalRotator.Yaw = GetControlRotation().Yaw + GetAnimCurveValue(FName("YawOffset"));
					SmoothCharacterRotation(LocalRotator,500,CalculateGroundedRotationRate());
				}
				break;
			case ERotationMode_ZMJ::Aiming:
				LocalRotator.Yaw = GetControlRotation().Yaw;
				SmoothCharacterRotation(LocalRotator,1000,20);
				break;
			}
		}
		else
		{
			// 没有移动的情况下
			switch (ViewMode)
			{
			case EViewMode_ZMJ::ThirdPerson:
				if (RotationMode == ERotationMode_ZMJ::Aiming)
				{
					LimitRotation(-100.0f,100.0f,20.0f);
				}
				break;
			case EViewMode_ZMJ::FirstPerson:
				LimitRotation(-100.0f,100.0f,20.0f);
				break;
			}

			auto LocalRotationAmount = GetAnimCurveValue(FName("RotationAmount"));
			if (FMath::Abs(LocalRotationAmount) > 0.001)
			{
				auto Delta = GetWorld()->GetDeltaSeconds();
				AddActorWorldRotation(FRotator(0.0f,0.0f,Delta*30.0f*LocalRotationAmount));
			}
		}
		break;
	}
}
// 插值目标旋转额外平滑的旋转行为
void ACharacter_ZMJ::SmoothCharacterRotation(FRotator Target, const float TargetInterpSpeed, float ActorInterpSpeed)
{
	auto LocalDelta = GetWorld()->GetDeltaSeconds();
	TargetRotation = FMath::RInterpConstantTo(TargetRotation,Target,LocalDelta,TargetInterpSpeed);

	// 第二次过渡 (可能是为了第三人称服务的
	// )
	auto LocalRotation = FMath::RInterpTo(GetActorRotation(),TargetRotation,LocalDelta,ActorInterpSpeed);

	SetActorRotation(LocalRotation);
}

// 防止角色旋转超过特定角度。
void ACharacter_ZMJ::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed)
{
	auto LocalControlRotation = GetControlRotation();
	auto LocalActorRotation = GetActorRotation();
	FRotator Delta = LocalControlRotation - LocalActorRotation;
	Delta.Normalize();

	if (!(Delta.Yaw >= AimYawMin && Delta.Yaw >= AimYawMax))
	{
		auto LocalYaw = 0.0f;
		if (Delta.Yaw > 0.0f)
		{
			LocalYaw = LocalControlRotation.Yaw + AimYawMin;
		}
		else
		{
			LocalYaw = LocalControlRotation.Yaw + AimYawMax;
		}

		auto LocalRotator = FRotator::ZeroRotator;
		LocalRotator.Yaw = LocalYaw;
		SmoothCharacterRotation(LocalRotator,0.0f,InterpSpeed);
	}
}

// 在移动设置中使用当前的旋转速率曲线来计算旋转速率。将曲线与映射的速度结合使用，您可以对每个速度的旋转速率进行高水平的控制。如果相机旋转得比较快，请提高速度。
float ACharacter_ZMJ::CalculateGroundedRotationRate()
{
	auto LocalRotationRate = CurrentMovementSettings.RotationRateCurve->GetFloatValue(GetMappedSpeed());
	auto LocalScale = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 300.f), FVector2D(1.0f, 3.0f), AimYawRate);
	return LocalRotationRate*LocalScale;
}

bool ACharacter_ZMJ::CanUpdateMovingRotation()
{
	return (((IsMoving && HasMovementInput) || Speed > 150 ) && !HasAnyRootMotion());
}

void ACharacter_ZMJ::DrawDebugShapes()
{
	// if (true)
	// {
	// 	return;
	// }
	/*FVector LineStart = FVector::ZeroVector;
	FVector LineEnd = FVector::ZeroVector;
	FVector LocalVelocity = GetVelocity();
	FColor LocalColor = FColor::Red;

	LineStart = GetActorLocation() - FVector(0.0f,0.0f,GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	auto LocalVelocityZero = LocalVelocity.Equals(FVector::ZeroVector,0.0001f);
	auto t = LocalVelocityZero ? LastVelocityRotation.Vector() : LocalVelocity;
	LineEnd = LineStart + t.GetUnsafeNormal() * FMath::GetMappedRangeValueClamped(FVector2D(0, GetCharacterMovement()->MaxWalkSpeed), FVector2D(50, 75), LocalVelocity.Size());
	LocalColor = LocalVelocityZero ? FColor(0.25f,0.0f,0.25f) : LocalVelocity;*/
	// DrawDebugDirectionalArrow(GetWorld(),LineStart,LineEnd,60,)
	// GetWorld()->SpawnActor()
}

void ACharacter_ZMJ::SetViewMode(EViewMode_ZMJ NewViewMode)
{
	if (NewViewMode != ViewMode)
	{
		OnViewModeChanged(NewViewMode);
	}
}

void ACharacter_ZMJ::SetGait(EGait_ZMJ NewGait)
{
	if (NewGait != Gait)
	{
		OnGaitChanged(NewGait);
	}
}

void ACharacter_ZMJ::SetRotationMode(ERotationMode_ZMJ NewRotationMode)
{
	if (NewRotationMode != RotationMode)
	{
		OnRotationModeChanged(NewRotationMode);
	}
}

void ACharacter_ZMJ::SetMovementState(EMovementState_ZMJ NewMovementState)
{
	if (MovementState != NewMovementState)
	{
		OnMovementStateChanged(NewMovementState);
	}
}

bool ACharacter_ZMJ::GetEssentialValues(FCharacterInformation_ZMJ& OutCharacterInformation)
{
	OutCharacterInformation.Velocity			= GetVelocity();
	OutCharacterInformation.Acceleration		= Acceleration;
	OutCharacterInformation.MovementInput		= GetCharacterMovement()->GetCurrentAcceleration();
	OutCharacterInformation.IsMoving			= IsMoving;
	OutCharacterInformation.HasMovementInput	= HasMovementInput;
	OutCharacterInformation.Speed				= Speed;
	OutCharacterInformation.MovementInputAmount	= MovementInputAmount;
	OutCharacterInformation.AimingRotation		= GetControlRotation();
	OutCharacterInformation.AimYawRate			= AimYawRate;
	return true;
}

bool ACharacter_ZMJ::GetCurrentStates(FCharacterStates_ZMJ& OutCharacterStates)
{
	OutCharacterStates.PawnMovementMode		= GetCharacterMovement()->MovementMode;
	OutCharacterStates.MovementState		= MovementState;
	OutCharacterStates.PrevMovementState	= PrevMovementState;
	OutCharacterStates.MovementAction		= MovementAction;
	OutCharacterStates.RotationMode			= RotationMode;
	OutCharacterStates.ActualGait			= Gait;
	OutCharacterStates.ActualStance			= Stance;
	OutCharacterStates.ViewMode				= ViewMode;
	OutCharacterStates.OverlaySate			= OverlayState;
	return  true;
}
