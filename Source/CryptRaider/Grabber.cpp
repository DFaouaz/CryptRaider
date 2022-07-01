// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	/*FRotator rotation = GetComponentRotation();
	FString rotString = rotation.ToCompactString();
	UE_LOG(LogTemp, Display, TEXT("Grabber rotation: %s"), *rotString);

	UWorld* world = GetWorld();
	float time = world->TimeSeconds;*/

	UPhysicsHandleComponent *physicsHandle = GetPhysicsHandle();
	if (physicsHandle == nullptr)
		return;

	if (physicsHandle->GetGrabbedComponent() != nullptr)
	{
		FVector targetLocation = GetComponentLocation() + GetForwardVector() * holdDistance;
		physicsHandle->SetTargetLocationAndRotation(targetLocation, GetComponentRotation());
	}
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent *physicsHandle = GetPhysicsHandle();
	if (physicsHandle == nullptr)
		return;

	FHitResult result;
	if (GetGrabbableInReach(result))
	{
		UPrimitiveComponent *hitComponent = result.GetComponent();
		// wake up rigidbodys
		hitComponent->SetSimulatePhysics(true);
		hitComponent->WakeAllRigidBodies();
		AActor* actor = result.GetActor();
		actor->Tags.Add("Grabbed");
		actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		physicsHandle->GrabComponentAtLocationWithRotation(
			hitComponent,
			NAME_None,
			result.ImpactPoint,
			GetComponentRotation());
	}
}

void UGrabber::Release()
{
	UPhysicsHandleComponent *physicsHandle = GetPhysicsHandle();
	if (physicsHandle == nullptr)
		return;

	if (physicsHandle->GetGrabbedComponent() != nullptr)
	{
		physicsHandle->GetGrabbedComponent()->GetOwner()->Tags.Remove("Grabbed");
		physicsHandle->ReleaseComponent();
	}
}

UPhysicsHandleComponent *UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent *result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (result == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grabber requires a PhysicsHandleComponent"));
	}
	return result;
}

bool UGrabber::GetGrabbableInReach(FHitResult& outResult) const
{
	UPhysicsHandleComponent *physicsHandle = GetPhysicsHandle();
	if (physicsHandle == nullptr)
		return false;

	FVector start = GetComponentLocation();
	FVector end = start + GetForwardVector() * maxGrabDistance;
	DrawDebugLine(GetWorld(), start, end, FColor::Red);
	DrawDebugSphere(GetWorld(), end, 10, 10, FColor::Blue, false, 5);

	FCollisionShape sphere = FCollisionShape::MakeSphere(grabRadius);
	return GetWorld()->SweepSingleByChannel(
		outResult,
		start,
		end,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		sphere);
}
