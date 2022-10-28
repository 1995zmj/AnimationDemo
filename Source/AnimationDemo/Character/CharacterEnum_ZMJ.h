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

UENUM()
enum EMyEnum
{
	None,
	Grounded,
	InAir,
	Mantling,
	Ragdoll,
};
