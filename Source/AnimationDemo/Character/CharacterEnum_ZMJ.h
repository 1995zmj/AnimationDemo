#pragma once
#include "CoreMinimal.h"
#include "CharacterEnum_ZMJ.generated.h"

// USTRUCT()
// struct ANIMATIONDEMO_API FTransRule
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY()
// 	TMap<FName, FName> KeyWord_mapping_TargetState;
//
// 	UPROPERTY()
// 	FName FromState;
// };
//
// USTRUCT()
// struct ANIMATIONDEMO_API FFSMData
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY()
// 	TMap<FName, FTransRule> CurrState_Mapping_TransRule;
// };

UENUM(BlueprintType)
enum EGait_ZMJ
{
	Walking,
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
	Pistol 1H,
	Pistol 2H,
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


