// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStruct_ZMJ.h"
#include "CharacterEnum_ZMJ.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Character_ZMJ.generated.h"

UCLASS()
class ANIMATIONDEMO_API ACharacter_ZMJ : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter_ZMJ();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);
	
	// EssentialInformation
	void SetEssentialValues();
	FVector CacheValues();
	FVector CalculateAcceleration();
	// StatChanges
	void OnBeginPlay();
	void OnGaitChanged(EGait_ZMJ NewActualGait);
	void OnRotationModeChanged(ERotationMode_ZMJ NewRotaionMode);
	void OnViewModeChanged(EViewMode_ZMJ NewViewMode);
	void OnOverlayStateChanged(EOverlayState_ZMJ NewOverlayState);

	// MovementSystem
	void SetMovementModel();
	void UpdateCharacterMovement();
	void UpdateDynamicMovementSetting(EGait_ZMJ AllowedGait);
	FMovementSettings_ZMJ GetTargetMovementSettings();
	EGait_ZMJ GetAllowedGait();
	EGait_ZMJ GetActualGait(EGait_ZMJ AllowedGait);
	bool CanSprint();
	float GetMappedSpeed();
	// RotationSystem
	void UpdateGroudedRotation();
	// CharacterStates Interfaces
	void SetViewMode(EViewMode_ZMJ NewViewMode);
	void SetGait(EGait_ZMJ NewGait);
	void SetRotationMode(ERotationMode_ZMJ NewRotationMode);
public:
	// References
	UAnimInstance* MainAnimInstance;
	// Input
	EGait_ZMJ DesiredGait;
	ERotationMode_ZMJ DesiredRotationMode;
	EStance_ZMJ DesiredStance;
	//EssentialInformation
	FVector Acceleration;
	bool IsMoving;
	bool HasMovementInput;
	FRotator LastVelocityRotation;
	FRotator LastMovementInputRotation;
	float Speed;
	float MovementInputAmount;
	float AimYawRate;
	// StateValues
	EMovementState_ZMJ MovementState;
	ERotationMode_ZMJ RotationMode;
	EGait_ZMJ Gait;
	EStance_ZMJ Stance;
	EViewMode_ZMJ ViewMode;
	EOverlayState_ZMJ OverlayState;
	//MovementSystem
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FDataTableRowHandle MovementModel;
	FMovementSettings_State_ZMJ* MovementData;
	FMovementSettings_ZMJ CurrentMovementSettings;
	// RotationSystem
	FRotator TargetRotation;
	FRotator InAirRotation;
	float YawOffset;
	// CachedVariables
	FVector PreviousVelocity;
	float PreviousAimYaw;
};
