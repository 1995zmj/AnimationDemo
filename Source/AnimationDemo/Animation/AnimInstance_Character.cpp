// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance_Character.h"
#include "GameFramework/Character.h"

void UAnimInstance_Character::OnInitiaizeAnimation()
{
	auto PawnOwner = TryGetPawnOwner();
	if (IsValid(PawnOwner))
	{
		auto LocalCharacter = Cast<ACharacter>(PawnOwner);
		if (IsValid(LocalCharacter))
		{
			Character = LocalCharacter;
		}
	}
}

void UAnimInstance_Character::OnUpdateAnimation(float DeltaTimeX)
{
	MyDeltaTimeX = DeltaTimeX;

	if (MyDeltaTimeX > 0 && IsValid(Character))
	{
		
	}
}

void UAnimInstance_Character::UpdateCharacterInfo()
{
	
}
