// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstanceEnum_ZMJ.h"
#include "AnimInstanceStruct_ZMJ.h"
#include "Animation/AnimInstance.h"
#include "AnimationDemo/Character/CharacterStruct_ZMJ.h"
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

	// UpdateValues
	void UpdateCharacterInfo();
	void UpdateAimingValues();
	void UpdateLayerValues();
	void UpdateFootIK();
	void UpdateMovementValues();
	void UpdateRotationValues();
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

	// Rotation
	EMovementDirection_ZMJ CalculateMovementDirection();
	EMovementDirection_ZMJ CalculateQuadrant(EMovementDirection_ZMJ Current, float FR_Threshold, float FL_Threshold, float BR_Threshold, float BL_Threshold,
	float Buffer, float Angel);
	bool AngleInRange(float Angle, float MinAngel, float MaxAngel, float Buffer, bool IncreaseBuffer);
	// Interpolation
	FVelocityBlend_ZMJ InterpVelocityBlend(FVelocityBlend_ZMJ Current, FVelocityBlend_ZMJ Target, float InterpSpeed, float DeltaTime);
	FLeanAmount_ZMJ InterpLeanAmount(FLeanAmount_ZMJ Current, FLeanAmount_ZMJ Target, float InterpSpeed, float DeltaTime);


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

	// Anim Graph - Aiming Values
	FRotator SmoothedAimingRotation;
	FRotator SpineRotation;
	FVector2D AimingAngle;
	FVector2D SmoothedAimingAngle;
	float AimSweepTime;
	float InputYawOffsetTime;
	float ForwardYawTime;
	float LeftYawTime;
	float RightYawTime;

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
	class UCurveVector* YawOffset_FB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveVector* YawOffset_LR;
	// Config
	float AnimatedWalkSpeed;
	float AnimatedRunSpeed;
	float AnimatedSprintSpeed;
	float SmoothedAimingRotationInterpSpeed;
	float VelocityBlendInterpSpeed;
	float GroundedLeanInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TriggerPivotSpeedLimit;
};

