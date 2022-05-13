// Fill out your copyright notice in the Description page of Project Settings.


#include "MySceneComponent.h"

UMySceneComponent::UMySceneComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	FieldOfView = 90.0f;
	AspectRatio = 1.777778f;
	OrthoWidth = 512.0f;
	OrthoNearClipPlane = 0.0f;
	OrthoFarClipPlane = WORLD_MAX;
	bConstrainAspectRatio = false;
}


