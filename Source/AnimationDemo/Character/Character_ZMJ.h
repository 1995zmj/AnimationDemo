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

	// StatChanges
	void OnBeginPlay();
	void OnGaitChanged(EGait_ZMJ NewActualGait);
	void OnRotationModeChanged(ERotationMode_ZMJ NewRotaionMode);
	void OnViewModeChanged(EViewMode_ZMJ NewViewMode);
	void OnOverlayStateChanged();

	// MovementSystem
	void SetMovementModel();

	// CharacterStates Interfaces
	void SetViewMode(EViewMode_ZMJ NewViewMode);
	void SetRotationMode(ERotationMode_ZMJ NewRotationMode);
public:
	// References
	UAnimInstance* MainAnimInstance;
	// Input
	EGait_ZMJ DesiredGait;
	ERotationMode_ZMJ DesiredRotationMode;
	EStance_ZMJ DesiredStance;
	// StateValues
	ERotationMode_ZMJ RotationMode;
	EGait_ZMJ Gait;
	EStance_ZMJ Stance;
	EViewMode_ZMJ ViewMode;
	EOverlayState_ZMJ OverlayState;
	//MovementSystem
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FDataTableRowHandle MovementModel;

	FMovementSettings_ZMJ* MovementData;
};
