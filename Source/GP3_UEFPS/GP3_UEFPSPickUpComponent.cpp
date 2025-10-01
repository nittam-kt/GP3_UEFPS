// Copyright Epic Games, Inc. All Rights Reserved.

#include "GP3_UEFPSPickUpComponent.h"

UGP3_UEFPSPickUpComponent::UGP3_UEFPSPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UGP3_UEFPSPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UGP3_UEFPSPickUpComponent::OnSphereBeginOverlap);
}

void UGP3_UEFPSPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AGP3_UEFPSCharacter* Character = Cast<AGP3_UEFPSCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
