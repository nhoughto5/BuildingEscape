// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape.h"
#include "Grabber.h"
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber() :
	reach(100.f),
	PhysicsHandle(nullptr),
	InputComponent(nullptr)
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
	}
	else {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}



const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// Get Player viewpoint
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerViewPointRotation);

	/// Draw red trace
	FVector LineTraceEnd = playerViewPointLocation + playerViewPointRotation.Vector() * reach;
	/// Setup Query Parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		playerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	/// Find what we hit.
	AActor* ActorHit = Hit.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Contact with: %s"), *(ActorHit->GetName()));
	}
	return Hit;
}


void UGrabber::FindPhysicsHandleComponent()
{
	/// Look for attached Physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("Grabber unable to find physics handle on %s."), *(GetOwner()->GetName()));
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	/// Get Player viewpoint
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerViewPointRotation);

	/// Draw red trace
	FVector LineTraceEnd = playerViewPointLocation + playerViewPointRotation.Vector() * reach;
	if (PhysicsHandle->GrabbedComponent) {
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}

void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"))
	FHitResult Hit = GetFirstPhysicsBodyInReach();
	auto GrabTarget = Hit.GetComponent();
	auto ActorHit = Hit.GetActor();
	if (ActorHit) {
		PhysicsHandle->GrabComponent(GrabTarget, NAME_None, GrabTarget->GetOwner()->GetActorLocation(), true);
	}
}
void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"))
	PhysicsHandle->ReleaseComponent();
}