// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PlayerCameraManager_ZMJ.generated.h"

/**
 * 
 */
UCLASS()
class ANIMATIONDEMO_API APlayerCameraManager_ZMJ : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetCameraMode(FName NewCamMode);
};
