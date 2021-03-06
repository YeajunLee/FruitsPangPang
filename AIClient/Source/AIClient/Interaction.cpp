// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/World.h"

// Sets default values
AInteraction::AInteraction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	RootComponent = CollisionVolume;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AInteraction::BeginPlay()
{
	Super::BeginPlay();
	
	//AddDynamic can't find intellisense but dont care bout that. 
	//인텔리센스에 확인 안되는 것 뿐이지 컴파일엔 상관없음
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteraction::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteraction::OnOverlapEnd);

}

// Called every frame
void AInteraction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AInteraction::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AInteraction::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
