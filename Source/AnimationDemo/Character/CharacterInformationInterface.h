// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStruct_ZMJ.h"
#include "UObject/Interface.h"
#include "CharacterInformationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterInformationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ANIMATIONDEMO_API ICharacterInformationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool GetEssentialValues(FCharacterInformation_ZMJ& OutCharacterInformation) = 0;

	virtual bool GetCurrentStates(FCharacterStates_ZMJ& OutCharacterStates) = 0;
};
