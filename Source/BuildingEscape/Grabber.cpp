// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape.h"
#include "Grabber.h"
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber() :
	reach(100.f),
	PhysicsHandle(nullptr),
	InputComponent(nullptr),
	CurrentlyHeldActor(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	FindInputComponent();
}

void UGrabber::FindInputComponent()
{
	/// Only Appears at runtime
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!InputComponent) {
		UE_LOG(LogTemp, Error, TEXT("Grabber unable to find input component on %s."), *(GetOwner()->GetName()));
		return;
	}
	else {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// Setup Query Parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	return HitResult;
}

void UGrabber::FindPhysicsHandleComponent()
{
	/// Look for attached Physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle || PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Grabber unable to find physics handle on %s."), *(GetOwner()->GetName()));
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}
}

void UGrabber::Grab() {
	FHitResult Hit = GetFirstPhysicsBodyInReach();
	auto GrabTarget = Hit.GetComponent(); //Get the mesh we hit
	CurrentlyHeldActor = Hit.GetActor();
	if (CurrentlyHeldActor) {
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponent(GrabTarget, NAME_None, GrabTarget->GetOwner()->GetActorLocation(), true);

		//Turn on Statue Light
		if (CurrentlyHeldActor->ActorHasTag("LightStatue")) {
			LightOn.Broadcast();
		}
	}
}
void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"));
	if (!PhysicsHandle) { return; }

	//Turn Off Light on Statue
	if (!CurrentlyHeldActor) {
		return;
	}
	if (CurrentlyHeldActor->ActorHasTag("LightStatue")) {
		LightOff.Broadcast();
	}

	PhysicsHandle->ReleaseComponent();
	CurrentlyHeldActor = nullptr;
}

FVector UGrabber::GetReachLineStart() {
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerViewPointRotation);
	return playerViewPointLocation;
}
FVector UGrabber::GetReachLineEnd() {
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerViewPointRotation);
	return playerViewPointLocation + playerViewPointRotation.Vector() * reach;
}