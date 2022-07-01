// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

UTriggerComponent::UTriggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* actor = GetAcceptableActor();
	mover->SetShouldMove(actor != nullptr);
	if (actor != nullptr)
	{
		UPrimitiveComponent* component = Cast<UPrimitiveComponent>(actor->GetRootComponent());	// cast root component
		if (component != nullptr) {
			component->SetSimulatePhysics(false);	// stop simulating physics
		}

		actor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform); // attach component to another component
	}

}

void UTriggerComponent::SetMover(UMover* nMover)
{
	mover = nMover;
}

AActor* UTriggerComponent::GetAcceptableActor() const
{
	TArray<AActor*> actors;
	GetOverlappingActors(actors);

	for (AActor* actor : actors)
	{
		if (actor->ActorHasTag(checkTag) && !actor->ActorHasTag("Grabbed"))
		{
			return actor;
		}
	}
	return nullptr;
}
