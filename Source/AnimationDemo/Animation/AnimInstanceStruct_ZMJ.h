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