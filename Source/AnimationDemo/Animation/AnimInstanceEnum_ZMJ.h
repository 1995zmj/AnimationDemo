#pragma once
#include "CoreMinimal.h"
#include "AnimInstanceEnum_ZMJ.generated.h"

UENUM(BlueprintType)
enum class EMovementDirection_ZMJ: uint8
{
	Forward UMETA(DisplayName = "Forward"),
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left"),
	Backward UMETA(DisplayName = "Backward"),
};

UENUM(BlueprintType)
enum class EHipsDirection_ZMJ: uint8
{
	F UMETA(DisplayName = "F"),
	B UMETA(DisplayName = "B"),
	RF UMETA(DisplayName = "RF"),
	RB UMETA(DisplayName = "RB"),
	LF UMETA(DisplayName = "LF"),
	LB UMETA(DisplayName = "LB"),
};

