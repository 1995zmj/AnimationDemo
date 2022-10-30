#pragma once
#include "CoreMinimal.h"
#include "CharacterEnum_ZMJ.h"
#include "Curves/CurveVector.h"
#include "Engine/DataTable.h"
#include "CharacterStruct_ZMJ.generated.h"

USTRUCT(BlueprintType)
struct ANIMATIONDEMO_API FMovementSettings_ZMJ : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float WalkSpeed = 0.0f;
	
	UPROPERTY(EditAnywhere)
	float RunSpeed = 0.0f;
	
	UPROPERTY(EditAnywhere)
	float SprintSpeed = 0.0f;
	
	UPROPERTY(EditAnywhere)
	UCurveVector* MovementCurve;
	
	UPROPERTY(EditAnywhere)
	UCurveFloat* RotationRateCurve;
};

USTRUCT(BlueprintType)
struct ANIMATIONDEMO_API FMovementSettings_Stance_ZMJ : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FMovementSettings_ZMJ Standing;
	
	UPROPERTY(EditAnywhere)
	FMovementSettings_ZMJ Crouching;

};


USTRUCT(BlueprintType)
struct ANIMATIONDEMO_API FMovementSettings_State_ZMJ : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FMovementSettings_Stance_ZMJ VelocityDirection;
	
	UPROPERTY(EditAnywhere)
	FMovementSettings_Stance_ZMJ LookingDirection;
	
	UPROPERTY(EditAnywhere)
	FMovementSettings_Stance_ZMJ Aiming;
};

USTRUCT(BlueprintType)
struct ANIMATIONDEMO_API FCharacterInformation_ZMJ
{
	GENERATED_BODY()
public:
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
};

USTRUCT(BlueprintType)
struct ANIMATIONDEMO_API FCharacterStates_ZMJ
{
	GENERATED_BODY()
public:
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
};

