#pragma once
#include "CoreMinimal.h"
#include "CharacterEnum_ZMJ.generated.h"

UENUM(BlueprintType)
enum class EGait_ZMJ: uint8
{
	Walking UMETA(DisplayName = "Walking"),
	Runing UMETA(DisplayName = "Runing"),
	Sprinting UMETA(DisplayName = "Sprinting"),
};

UENUM(BlueprintType)
enum class ERotationMode_ZMJ: uint8
{
	VelocityDirection UMETA(DisplayName = "VelocityDirection"),
	LookingDirection UMETA(DisplayName = "LookingDirection"),
	Aiming UMETA(DisplayName = "Aiming"),
};

UENUM(BlueprintType)
enum class EViewMode_ZMJ: uint8
{
	ThirdPerson UMETA(DisplayName = "ThirdPerson"),
	FirstPerson UMETA(DisplayName = "FirstPerson"),
};

// 有很多动作的覆盖姿势
UENUM(BlueprintType)
enum class EOverlayState_ZMJ: uint8
{
	Default UMETA(DisplayName = "Default"),
	Masculine UMETA(DisplayName = "Masculine"),
	Feminine UMETA(DisplayName = "Feminine"),
	Injured UMETA(DisplayName = "Injured"),
	Rifle UMETA(DisplayName = "Rifle"),
	Pistol_1H UMETA(DisplayName = "Pistol_1H"),
	Pistol_2H UMETA(DisplayName = "Pistol_2H"),
	Bow UMETA(DisplayName = "Bow"),
	Torch UMETA(DisplayName = "Torch"),
	Binoculars UMETA(DisplayName = "Binoculars"),
	Box UMETA(DisplayName = "Box"),
	Barrel UMETA(DisplayName = "Barrel"),
};

UENUM(BlueprintType)
enum class EStance_ZMJ: uint8
{
	Standing UMETA(DisplayName = "Standing"),
	Crouching UMETA(DisplayName = "Crouching"),
};

UENUM(BlueprintType)
enum class EMovementState_ZMJ: uint8
{
	None UMETA(DisplayName = "None"),
	Grounded UMETA(DisplayName = "Grounded"),
	InAir UMETA(DisplayName = "InAir"),
	Mantling UMETA(DisplayName = "Mantling"),
	Ragdoll UMETA(DisplayName = "Ragdoll"),
};

UENUM(BlueprintType)
enum class EMovementAction_ZMJ: uint8
{
	None UMETA(DisplayName = "None"),
	LowMantle UMETA(DisplayName = "LowMantle"),
	HighMantle UMETA(DisplayName = "HighMantle"),
	Rolling UMETA(DisplayName = "Rolling"),
	GettingUp UMETA(DisplayName = "GettingUp"),
};


