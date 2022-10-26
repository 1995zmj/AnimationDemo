#pragma once
#include "CoreMinimal.h"
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

