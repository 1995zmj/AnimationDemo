// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PlayerCameraManager_ZMJ.generated.h"

/**
 * 
 */
UCLASS()
class ANIMATIONDEMO_API APlayerCameraManager_ZMJ : public  APlayerCameraManager
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMeshComponent* CameraBehavior;
	
	UFUNCTION(BlueprintCallable)
	void SetCameraMode(FName NewCamMode);

	UFUNCTION(BlueprintCallable)
	void OnPossess(APawn* NewPawn);

	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeeds, float DeltaTime);
	
	FTransform SmoothedPivotTarget;
	FVector PivotLocation;
	
	FRotator TargetCameraRotation;
	FVector TargetCameraLocation;
protected:
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;
	void CustomCameraBehavior(FTViewTarget& OutVT, float DeltaTime);

	float GetCameraBehaviorParam(FName CurveName);

	FTransform Get3PPivotTarget();
	FVector GetFPCameraTarget();
	
private:
	APawn* ControlledPawn;
};
