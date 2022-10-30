// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStruct_ZMJ.h"
#include "CharacterEnum_ZMJ.h"
#include "CharacterInformationInterface.h"
#include "GameFramework/Character.h"
#include "Character_ZMJ.generated.h"

UCLASS()
class ANIMATIONDEMO_API ACharacter_ZMJ : public ACharacter, public ICharacterInformationInterface
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
	// PlayerInputGraph
	// 运动输入
	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);
	// 相机输入
	UFUNCTION()
	void LookUp(float Value);
	UFUNCTION()
	void LookRight(float Value);
	
	// Utility
	FVector GetControlForwardVector();
	FVector GetControlRightVector();
	float GetAnimCurveValue(FName CurveName);
	// Input
	void PlayerMovementInput(bool IsForwardAxis, float AxisValue);
	float FixDiagonalGamepadValues(float AxisValue);
	// EssentialInformation
	void SetEssentialValues();
	void CacheValues();
	FVector CalculateAcceleration();
	// StatChanges
	void OnBeginPlay();
	UFUNCTION()
	void OnMovementModeChangedDDelegateEvent(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
	void OnCharacterMovementModeChanged(EMovementMode PrevMovementMode, EMovementMode NewMovementMode, uint8 PrevCustomMode, uint8 NewCustomMode);
	void OnMovementStateChanged(EMovementState_ZMJ NewMovementState);
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
	void SmoothCharacterRotation(FRotator Target,const float TargetInterpSpeed, float ActorInterpSpeed);
	void LimitRotation(float AimYawMin,float AimYawMax,float InterpSpeed);
	float CalculateGroundedRotationRate();
	bool CanUpdateMovingRotation();
	// Debug
	void DrawDebugShapes();
	// CharacterStates Interfaces
	void SetViewMode(EViewMode_ZMJ NewViewMode);
	void SetGait(EGait_ZMJ NewGait);
	void SetRotationMode(ERotationMode_ZMJ NewRotationMode);
	void SetMovementState(EMovementState_ZMJ NewMovementState);

	// CharacterInformation
	UFUNCTION(BlueprintCallable)
	virtual bool GetEssentialValues(FCharacterInformation_ZMJ& OutCharacterInformation) override;
	virtual bool GetCurrentStates(FCharacterStates_ZMJ& OutCharacterStates) override;
public:
	// References
	UAnimInstance* MainAnimInstance;
	// Input
	EGait_ZMJ DesiredGait;
	ERotationMode_ZMJ DesiredRotationMode; 
	EStance_ZMJ DesiredStance;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float LookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookRightRate;
	// EssentialInformation
	// 真实的加速度
	FVector Acceleration;
	bool IsMoving;
	bool HasMovementInput;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRotator LastVelocityRotation;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRotator LastMovementInputRotation;
	float Speed;
	float MovementInputAmount;
	float AimYawRate;
	// StateValues
	EMovementState_ZMJ MovementState;
	EMovementState_ZMJ PrevMovementState;
	EMovementAction_ZMJ MovementAction;
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
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRotator TargetRotation;
	FRotator InAirRotation;
	float YawOffset;
	// CachedVariables
	FVector PreviousVelocity;
	float PreviousAimYaw;
};
