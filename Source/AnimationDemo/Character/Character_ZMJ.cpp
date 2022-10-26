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

void ACharacter_ZMJ::OnGaitChanged(EGait_ZMJ NewActualGait)
{
	Gait = NewActualGait;
}

void ACharacter_ZMJ::OnRotationModeChanged(ERotationMode_ZMJ NewRotaionMode)
{
	RotationMode = NewRotaionMode;
	if (RotationMode == ERotationMode_ZMJ::VelocityDirection)
	{
		if (ViewMode == EViewMode_ZMJ::FirstPerson)
		{
			SetViewMode(EViewMode_ZMJ::ThirdPerson);
		}
	}
}

void ACharacter_ZMJ::OnViewModeChanged(EViewMode_ZMJ NewViewMode)
{
	ViewMode = NewViewMode;
	switch (ViewMode)
	{
	case ThirdPerson:
		switch (RotationMode)
		{
		case VelocityDirection:
			SetRotationMode(DesiredRotationMode);
			break;
		case LookingDirection:
			SetRotationMode(DesiredRotationMode);
			break;
		case Aiming:
			break;
		default:
			break;
		}
		break;
	case FirstPerson:
		switch (RotationMode)
		{
		case VelocityDirection:
			SetRotationMode(ERotationMode_ZMJ::LookingDirection);
			break;
		case LookingDirection:
			break;
		case Aiming:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void ACharacter_ZMJ::SetMovementModel()
{
	// MovementModel.DataTable->GetTableData(MovementModel.RowName);
	// MovementModel.GetRow<>()
	FString ContextString;
	MovementData = MovementModel.GetRow<FMovementSettings_ZMJ>(ContextString);
}

void ACharacter_ZMJ::SetViewMode(EViewMode_ZMJ NewViewMode)
{
	if (NewViewMode != ViewMode)
	{
		ViewMode = NewViewMode;
		OnViewModeChanged(NewViewMode);
	}
	
}

void ACharacter_ZMJ::SetRotationMode(ERotationMode_ZMJ NewRotationMode)
{
	if (NewRotationMode != RotationMode)
	{
		RotationMode = NewRotationMode;
		OnRotationModeChanged(NewRotationMode);
	}
	
}


