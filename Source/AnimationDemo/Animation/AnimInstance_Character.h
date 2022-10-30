// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstance_Character.generated.h"

/**
 * 
 */
UCLASS()
class ANIMATIONDEMO_API UAnimInstance_Character : public UAnimInstance
{
	GENERATED_BODY()

public:
	// References
	class ACharacter* Character;
	float MyDeltaTimeX;



	void OnInitiaizeAnimation();

	void OnUpdateAnimation(float DeltaTimeX);

	// UpdateValues
	void UpdateCharacterInfo();
};

