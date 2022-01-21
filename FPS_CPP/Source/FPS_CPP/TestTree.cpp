// Fill out your copyright notice in the Description page of Project Settings.


#include "TestTree.h"

// Sets default values
ATestTree::ATestTree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ATestTree::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestTree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

