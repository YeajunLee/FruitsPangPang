// Fill out your copyright notice in the Description page of Project Settings.


#include "PointOfInterestComponent.h"
#include "MyCharacter.h"
#include "MainWidget.h"
#include "MiniMapWidget.h"
#include "Network.h"

// Sets default values for this component's properties
UPointOfInterestComponent::UPointOfInterestComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPointOfInterestComponent::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			
			if (Network::GetNetwork()->mMyCharacter != nullptr)
			{
				Network::GetNetwork()->mMyCharacter->mMainWidget->W_MiniMap_0->AddPOI(GetOwner());
			}
		}), 0.2, false);
}


// Called every frame
void UPointOfInterestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPointOfInterestComponent::InitializePOI()
{
	if (Network::GetNetwork()->mMyCharacter != nullptr)
	{
		Network::GetNetwork()->mMyCharacter->mMainWidget->W_MiniMap_0->AddPOI(GetOwner());
	}
}

