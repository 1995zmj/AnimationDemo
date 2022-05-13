// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class ANIMATIONDEMO_API AMyActor : public AActor
{
	GENERATED_BODY()
	
//// Aspect Ratio (Width/Height)
//UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = CameraSettings, meta = (ClampMin = "0.1", ClampMax = "100.0", EditCondition = "bConstrainAspectRatio"))
//float AspectRatio;
//UFUNCTION(BlueprintCallable, Category = Camera)
//void SetAspectRatio(float InAspectRatio) { AspectRatio = InAspectRatio; }

public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
