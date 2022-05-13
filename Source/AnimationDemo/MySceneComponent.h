// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MySceneComponent.generated.h"


UCLASS(HideCategories = (Mobility, Rendering, LOD), Blueprintable, ClassGroup = Camera, meta = (BlueprintSpawnableComponent))
class ANIMATIONDEMO_API UMySceneComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

		UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings, meta = (UIMin = "5.0", UIMax = "170", ClampMin = "0.001", ClampMax = "360.0", Units = deg))
		float FieldOfView;
	UFUNCTION(BlueprintCallable, Category = Camera)
		virtual void SetFieldOfView(float InFieldOfView) { FieldOfView = InFieldOfView; }

	/** The desired width (in world units) of the orthographic view (ignored in Perspective mode) */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
		float OrthoWidth;
	UFUNCTION(BlueprintCallable, Category = Camera)
		void SetOrthoWidth(float InOrthoWidth) { OrthoWidth = InOrthoWidth; }

	/** The near plane distance of the orthographic view (in world units) */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
		float OrthoNearClipPlane;
	UFUNCTION(BlueprintCallable, Category = Camera)
		void SetOrthoNearClipPlane(float InOrthoNearClipPlane) { OrthoNearClipPlane = InOrthoNearClipPlane; }

	/** The far plane distance of the orthographic view (in world units) */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
		float OrthoFarClipPlane;
	UFUNCTION(BlueprintCallable, Category = Camera)
		void SetOrthoFarClipPlane(float InOrthoFarClipPlane) { OrthoFarClipPlane = InOrthoFarClipPlane; }

	// Aspect Ratio (Width/Height)
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings, meta = (ClampMin = "0.1", ClampMax = "100.0", EditCondition = "bConstrainAspectRatio"))
		float AspectRatio;
	UFUNCTION(BlueprintCallable, Category = Camera)
		void SetAspectRatio(float InAspectRatio) { AspectRatio = InAspectRatio; }

	// If bConstrainAspectRatio is true, black bars will be added if the destination view has a different aspect ratio than this camera requested.
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraOptions)
		uint8 bConstrainAspectRatio : 1;
	UFUNCTION(BlueprintCallable, Category = Camera)
		void SetConstraintAspectRatio(bool bInConstrainAspectRatio) { bConstrainAspectRatio = bInConstrainAspectRatio; }


public:
	// The type of camera
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
		TEnumAsByte<ECameraProjectionMode::Type> ProjectionMode;
	UFUNCTION(BlueprintCallable, Category = Camera)
		void SetProjectionMode(ECameraProjectionMode::Type InProjectionMode) { ProjectionMode = InProjectionMode; }

	
};
