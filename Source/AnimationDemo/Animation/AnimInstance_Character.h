// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstanceEnum_ZMJ.h"
#include "AnimInstanceStruct_ZMJ.h"
#include "Animation/AnimInstance.h"
#include "AnimationDemo/Character/CharacterStruct_ZMJ.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AnimInstance_Character.generated.h"

/**
 * 
 */
UCLASS()
class ANIMATIONDEMO_API UAnimInstance_Character : public UAnimInstance
{
	GENERATED_BODY()

public:

	// 要蓝图中调用
	UFUNCTION(BlueprintCallable)
	void OnInitializeAnimation();
	UFUNCTION(BlueprintCallable)
	void OnUpdateAnimation(float DeltaTimeX);

	// EventGraph
	UFUNCTION(BlueprintCallable)
	void PlayTransition(FDynamicMontageParams_ZMJ Parameters);

	// UpdateValues
	void UpdateCharacterInfo();
	void UpdateAimingValues();
	void UpdateLayerValues();
	void UpdateFootIK();
	void UpdateMovementValues();
	void UpdateRotationValues();
	void UpdateInAirValues();
	// Grounded
	bool ShouldMoveCheck();
	bool CanTurnInPalce();
	bool CanRotateInPlace();
	bool CanDynamicTransition();
	void TurnInPlace(FRotator TargetRotation, float PlayRateScale, float StartTime, bool OverrideCurrent);
	void TurnInPlaceCheck();
	void RotateInPlaceCheck();
	void DynamicTransitionCheck();
	// Movement
	FVelocityBlend_ZMJ CalculateVelocityBlend();
	float CalculateDiagonalScaleAmount();
	FVector CalculateRelativeAccelerationAmount();
	float CalculateWalkRunBlend();
	float CalculateStrideBlend();
	float CalculateStandingPlayRate();
	float CalculateCrouchingPlayRate();
	// In Air
	float CalculateLandPrediction();
	FLeanAmount_ZMJ CalculateInAirLeanAmount();
	// Foot IK
	void SetFootOffsets(FName Enable_FootIK_Curve, FName IKFootBone, FName RootBone, FVector& CurrentLocationTarget, FVector& CurrentLocationOffset, FRotator& CurrentRotationOffset);
	void SetPelvisIKOffset(FVector FootOffset_L_Target,FVector FootOffset_R_Target);
	void SetFootLocking(FName Enable_FootIK_Curve, FName FootLockCurve, FName IKFootBone, float& CurrentFootLockAlpha, FVector& CurrentFootLockLocation, FRotator& CurrentFootLockRotation);
	void SetFootLockOffsets(FVector& LocalLocation, FRotator& LocalRotation);
	void ResetIKOffsets();
	// Rotation
	EMovementDirection_ZMJ CalculateMovementDirection();
	EMovementDirection_ZMJ CalculateQuadrant(EMovementDirection_ZMJ Current, float FR_Threshold, float FL_Threshold, float BR_Threshold, float BL_Threshold,
	float Buffer, float Angel);
	bool AngleInRange(float Angle, float MinAngel, float MaxAngel, float Buffer, bool IncreaseBuffer);
	// Interpolation
	FVelocityBlend_ZMJ InterpVelocityBlend(FVelocityBlend_ZMJ Current, FVelocityBlend_ZMJ Target, float InterpSpeed, float DeltaTime);
	FLeanAmount_ZMJ InterpLeanAmount(FLeanAmount_ZMJ Current, FLeanAmount_ZMJ Target, float InterpSpeed, float DeltaTime);
	// Debug
	EDrawDebugTrace::Type GetTraceDebugType(EDrawDebugTrace::Type ShowTraceType);
	// Interfaces
	void Jumped();
	// Macros
	float GetAnimCurve_Compact();
	float GetAnimCurve_Clamped(FName Name, float Bias, float ClmapMin, float ClampMax);
	float GetFloatValue(class UCurveFloat* CurveFloat,float InTime);
	FVector GetVectorValue(class UCurveVector* CurveVector,float InTime);
	float GetDistanceBetweenTwoSocketsAndMapRange(const USkeletalMeshComponent* Component, const FName SocketOrBoneNameA, ERelativeTransformSpace SocketSpaceA, const FName SocketOrBoneNameB, ERelativeTransformSpace SocketSpaceB,
		bool bRemapRange = false, float InRangeMin = 0.0f, float InRangeMax = 0.0f, float OutRangeMin = 0.0f, float OutRangeMax = 0.0f);
	
	// References
	UPROPERTY()
	class ACharacter* Character;
	float MyDeltaTimeX;
	// Character Information
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Acceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MovementInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsMoving;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasMovementInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementInputAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator AimingRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AimYawRate;
	// Character States
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TEnumAsByte<enum EMovementMode> PawnMovementMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementState_ZMJ MovementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementState_ZMJ PrevMovementState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementAction_ZMJ MovementAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERotationMode_ZMJ RotationMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGait_ZMJ ActualGait;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStance_ZMJ ActualStance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EViewMode_ZMJ ViewMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOverlayState_ZMJ OverlaySate;
	
	// AnimGraph - Grounded
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementDirection_ZMJ MovementDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHipsDirection_ZMJ TrackedHipsDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RelativeAccelerationAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Rotate_L;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Rotate_R;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Pivot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotateRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RotationScale;
	float DiagonalScaleAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkRunBlend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StandingPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchingPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StrdeBlend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVelocityBlend_ZMJ VelocityBlend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLeanAmount_ZMJ LeanAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RYaw;

	// Anim Graph - In Air
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpPlayRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FallSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LandPrediction;

	// Anim Graph - Aiming Values
	FRotator SmoothedAimingRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator SpineRotation;
	FVector2D AimingAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D SmoothedAimingAngle;
	float AimSweepTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputYawOffsetTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForwardYawTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LeftYawTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RightYawTime;

	// Anim Graph - Layer Blending
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Enable_AimOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BasePose_N;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BasePose_CLF;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpineAdd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HeadAdd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmLAdd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmRAdd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmL_LS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmR_LS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmL_MS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ArmR_MS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HandL;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HandR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Enable_HandIK_L;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Enable_HandIK_R;

	// Anim Graph - Foot IK
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FootLock_L_Alpha;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FootLock_L_Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator FootLock_L_Rotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FootLock_R_Alpha;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector FootLock_R_Location;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator FootLock_R_Rotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FootOffset_L_Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator FootOffset_L_Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FootOffset_R_Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator FootOffset_R_Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PelvisOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PelvisAlpha;
	
	// TurnInPlace
	float TurnCheckMinAngle;
	float Turn180Threshold;
	float AimYawRateLimit;
	float ElapsedDelayTime;
	float MinAngleDelay;
	float MaxAngleDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurnInPlace_Asset_ZMJ N_TurnIP_L90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurnInPlace_Asset_ZMJ N_TurnIP_R90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurnInPlace_Asset_ZMJ N_TurnIP_L180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurnInPlace_Asset_ZMJ N_TurnIP_R180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurnInPlace_Asset_ZMJ CLF_TurnIP_L90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurnInPlace_Asset_ZMJ CLF_TurnIP_R90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurnInPlace_Asset_ZMJ CLF_TurnIP_L180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurnInPlace_Asset_ZMJ CLF_TurnIP_R180;
	// RotateInPlace
	float RotateMinThreshold;
	float RotateMaxThreshold;
	float AimYawRateMinRange;
	float AimYawRateMaxRange;
	float MinPlayRate;
	float MaxPlayRate;
	// BlendCurves
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveFloat* DiagonalScaleAmountCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveFloat* StrideBlend_N_Walk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveFloat* StrideBlend_N_Run;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveFloat* StrideBlend_C_Walk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveFloat* LandPredictionCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveFloat* LeanInAirCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveVector* YawOffset_FB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveVector* YawOffset_LR;
	// Config
	float AnimatedWalkSpeed;
	float AnimatedRunSpeed;
	float AnimatedSprintSpeed;
	float AnimatedCrouchSpeed;
	float SmoothedAimingRotationInterpSpeed;
	float VelocityBlendInterpSpeed;
	float GroundedLeanInterpSpeed;
	float InAirLeanInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TriggerPivotSpeedLimit;
	float FootHeight;
	float IK_TraceDistanceAboveFoot;
	float IK_TraceDistanceBelowFoot;
};

