// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_ZMJ.h"

#include "K2Node_GetDataTableRow.h"

// Sets default values
ACharacter_ZMJ::ACharacter_ZMJ()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACharacter_ZMJ::BeginPlay()
{
	Super::BeginPlay();
	OnBeginPlay();
}

// Called every frame
void ACharacter_ZMJ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacter_ZMJ::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward/Backwards", this, &ACharacter_ZMJ::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight/Left", this, &ACharacter_ZMJ::MoveRight);

	PlayerInputComponent->BindAxis("LookLeft/Right", this, &ACharacter_ZMJ::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp/Down", this, &ACharacter_ZMJ::AddControllerPitchInput);
}


void ACharacter_ZMJ::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void ACharacter_ZMJ::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void ACharacter_ZMJ::OnBeginPlay()
{
	// 确保 actor 的参数更新完，在更新动画蓝图的
	auto TempMesh = GetMesh();
	TempMesh->AddTickPrerequisiteActor(this);

	MainAnimInstance = TempMesh->GetAnimInstance();

	SetMovementModel();
}

void ACharacter_ZMJ::SetMovementModel()
{
	// MovementModel.DataTable->GetTableData(MovementModel.RowName);
	// MovementModel.GetRow<>()
	FString ContextString;
	MovementData = MovementModel.GetRow<FMovementSettings_ZMJ>(ContextString);
}


