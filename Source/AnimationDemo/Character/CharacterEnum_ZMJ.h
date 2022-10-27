#pragma once
#include "CoreMinimal.h"
#include "CharacterEnum_ZMJ.generated.h"

UENUM(BlueprintType)
enum EGait_ZMJ
{
	Walking = 0,
	Runing,
	Sprinting,
};

UENUM(BlueprintType)
enum ERotationMode_ZMJ
{
	VelocityDirection,
	LookingDirection,
	Aiming,
};

UENUM(BlueprintType)
enum EViewMode_ZMJ
{
	ThirdPerson,
	FirstPerson,
};

// 有很多动作的覆盖姿势
UENUM(BlueprintType)
enum EOverlayState_ZMJ
{
	Default,
	Masculine,
	Feminine,
	Injured,
	Rifle,
	Pistol_1H,
	Pistol_2H,
	Bow,
	Torch,
	Binoculars,
	Box,
	Barrel,
};

UENUM(BlueprintType)
enum EStance_ZMJ
{
	Standing,
	Crouching,
};

UENUM(BlueprintType)
enum EMovementState_ZMJ
{
	None = 0,
	Grounded,
	InAir,
	Mantling,
	Ragdoll,
};

UENUM(BlueprintType)
enum class EMovementAction_ZMJ : uint8
{
	None = 0,
	LowMantle UMETA(DisplayName = "LowMantle"),
	HighMantle UMETA(DisplayName = "HighMantle"),
	Rolling UMETA(DisplayName = "Rolling"),
	GettingUp UMETA(DisplayName = "GettingUp"),
};


