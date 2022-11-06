#pragma once
#include "CoreMinimal.h"
#include "AnimInstanceStruct_ZMJ.generated.h"

USTRUCT(BlueprintType)
struct ANIMATIONDEMO_API FVelocityBlend_ZMJ
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float F = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float B = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float L = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float R = 0.0f;
};

USTRUCT(BlueprintType)
struct ANIMATIONDEMO_API FLeanAmount_ZMJ
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LR = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FB = 0.0f;
};