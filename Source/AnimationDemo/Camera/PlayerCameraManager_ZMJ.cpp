// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraManager_ZMJ.h"

#include "Kismet/KismetMathLibrary.h"

APlayerCameraManager_ZMJ::APlayerCameraManager_ZMJ(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraBehavior = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("CameraBehavior"));
	if (CameraBehavior)
	{
		CameraBehavior->SetupAttachment(RootComponent);
	}
}

void APlayerCameraManager_ZMJ::SetCameraMode(FName NewCamMode)
{
	CameraStyle = NewCamMode;
}

void APlayerCameraManager_ZMJ::OnPossess(APawn* NewPawn)
{
	ControlledPawn = NewPawn;
}

FVector APlayerCameraManager_ZMJ::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation,
	FRotator CameraRotation, FVector LagSpeeds, float DeltaTime)
{

	auto CameraRotationYaw = FRotator(0.0f, 0.0f, CameraRotation.Yaw);
	auto TempCurrentLocation = CameraRotationYaw.UnrotateVector(CurrentLocation);
	auto TempTargetLocation = CameraRotationYaw.UnrotateVector(TargetLocation);

	auto X = FMath::FInterpTo(TempCurrentLocation.X, TempTargetLocation.X, DeltaTime, LagSpeeds.X);
	auto Y = FMath::FInterpTo(TempCurrentLocation.Y, TempTargetLocation.Y, DeltaTime, LagSpeeds.Y);
	auto Z = FMath::FInterpTo(TempCurrentLocation.Z, TempTargetLocation.Z, DeltaTime, LagSpeeds.Z);
	return CameraRotationYaw.RotateVector(FVector(X, Y, Z));
}

void APlayerCameraManager_ZMJ::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	if (ControlledPawn)
	{
		CustomCameraBehavior(OutVT,DeltaTime);
	}
	else
	{
		Super::UpdateViewTargetInternal(OutVT, DeltaTime);
	}
}

void APlayerCameraManager_ZMJ::CustomCameraBehavior(FTViewTarget& OutVT, float DeltaTime)
{
	FTransform PivotTarget = Get3PPivotTarget();
	FVector FPTarget = GetFPCameraTarget();
	float TPFOV = 90.0f;
	float FPFOV = 90.0f;
 
	// 计算过渡角度
	auto Current = GetCameraRotation();
	auto Target = GetOwningPlayerController()->GetControlRotation();
	auto InterpSpeed = GetCameraBehaviorParam(FName("RotationLagSpeed"));
	TargetCameraRotation = FMath::RInterpTo(Current, Target, DeltaTime, InterpSpeed);
 
	// 计算过渡位置
	auto LagSpeeds = FVector(1.0f, 1.0f, 1.0f);
	LagSpeeds.X = GetCameraBehaviorParam(FName("PivotLagSpeed_X"));
	LagSpeeds.Y = GetCameraBehaviorParam(FName("PivotLagSpeed_Y"));
	LagSpeeds.Z = GetCameraBehaviorParam(FName("PivotLagSpeed_Z"));
	auto TempLocation = CalculateAxisIndependentLag(SmoothedPivotTarget.GetLocation(), PivotTarget.GetLocation(), TargetCameraRotation, LagSpeeds, DeltaTime);
	SmoothedPivotTarget = FTransform( PivotTarget.GetRotation(), TempLocation, FVector::OneVector);
 
	// 计算偏移值
	auto ForwardVector = SmoothedPivotTarget.GetRotation().GetForwardVector();
	auto RightVector = SmoothedPivotTarget.GetRotation().GetRightVector();
	auto UpVector = SmoothedPivotTarget.GetRotation().GetUpVector();
	auto PivotOffset_X = GetCameraBehaviorParam(FName("PivotOffset_X"));
	auto PivotOffset_Y = GetCameraBehaviorParam(FName("PivotOffset_Y"));
	auto PivotOffset_Z = GetCameraBehaviorParam(FName("PivotOffset_Z"));
	auto Offset = ForwardVector * PivotOffset_X + RightVector * PivotOffset_Y + UpVector * PivotOffset_Z;
	PivotLocation = (SmoothedPivotTarget.GetLocation() + Offset);

	//计算相机的实际位置
	ForwardVector = UKismetMathLibrary::GetForwardVector(TargetCameraRotation);
	RightVector = UKismetMathLibrary::GetRightVector(TargetCameraRotation);
	UpVector = UKismetMathLibrary::GetUpVector(TargetCameraRotation);
	auto CameraOffset_X = GetCameraBehaviorParam(FName("CameraOffset_X"));
	auto CameraOffset_Y = GetCameraBehaviorParam(FName("CameraOffset_Y"));
	auto CameraOffset_Z = GetCameraBehaviorParam(FName("CameraOffset_Z"));
	TargetCameraLocation = PivotLocation + ForwardVector * CameraOffset_X + RightVector * CameraOffset_Y + UpVector * CameraOffset_Z;

	// 检查碰撞，撞到墙，距离缩小，之后再考虑



	OutVT.POV.Location = TargetCameraLocation;
	OutVT.POV.Rotation = TargetCameraRotation;
	OutVT.POV.FOV = TPFOV;
}

float APlayerCameraManager_ZMJ::GetCameraBehaviorParam(FName CurveName)
{
	auto AnimInstacne = CameraBehavior->GetAnimInstance();
	if (IsValid(AnimInstacne))
	{
		return AnimInstacne->GetCurveValue(CurveName);
	}
	return 0;
}

FTransform APlayerCameraManager_ZMJ::Get3PPivotTarget()
{
	if (IsValid(ControlledPawn))
	{
		return ControlledPawn->GetActorTransform();
	}
	return FTransform::Identity;
}

FVector APlayerCameraManager_ZMJ::GetFPCameraTarget()
{
	if (IsValid(ControlledPawn))
	{
		return ControlledPawn->GetActorLocation();
	}
	return FVector::OneVector;
}

