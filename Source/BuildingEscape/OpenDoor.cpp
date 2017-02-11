// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape.h"
#include "OpenDoor.h"
#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor() :
	PressurePlate(nullptr),
	Owner(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("Pressure Plate missing on %s."), *(GetOwner()->GetName()));
	}
}

void UOpenDoor::CloseDoor()
{
	if (!Owner) { return; }
	Owner->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetTotalMassOfActorsOnPlate() > TriggerMass) {
		OnOpen.Broadcast();
	}
	else {
		OnClose.Broadcast();
	}
}

//bool inline UOpenDoor::IsTimeToCloseDoor()
//{
//	float currentTime = GetWorld()->GetTimeSeconds();
//	if ((currentTime - LastDoorOpenTime) >= DoorCloseDelay) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}

float UOpenDoor::GetTotalMassOfActorsOnPlate() {
	float totalMass = 0.f;
	/// Find all overlapping actors
	TArray<AActor*> Actors;
	if (!PressurePlate) { return totalMass; }
	PressurePlate->GetOverlappingActors(OUT Actors);
	/// Iterate through actors and sum mass
	for (const auto& actor : Actors) {
		totalMass += actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return totalMass;
}

