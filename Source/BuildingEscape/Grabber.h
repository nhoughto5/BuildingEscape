// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLightEvent);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrabber();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FLightEvent LightOn;

	UPROPERTY(BlueprintAssignable)
	FLightEvent LightOff;
private:
	float reach;
	UPhysicsHandleComponent* PhysicsHandle;
	UInputComponent* InputComponent;
	AActor *CurrentlyHeldActor;
	void Grab();
	void Release();
	void FindPhysicsHandleComponent();
	void FindInputComponent();
	const FHitResult GetFirstPhysicsBodyInReach();
	FVector GetReachLineEnd();
	FVector GetReachLineStart();
};
