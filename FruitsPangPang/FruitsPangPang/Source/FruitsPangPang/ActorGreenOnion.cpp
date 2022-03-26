// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorGreenOnion.h"

// Sets default values
AActorGreenOnion::AActorGreenOnion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("static mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GreenOnionAsset(TEXT("/Game/Assets/Fruits/BigGreenOnion/SM_GreenOnion.SM_GreenOnion"));
	if (GreenOnionAsset.Succeeded())
		staticMeshComponent->SetStaticMesh(GreenOnionAsset.Object);

	capsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("collision"));
	capsuleCollision->SetupAttachment(staticMeshComponent);

	capsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	capsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &AActorGreenOnion::OnOverlapBegin);
	capsuleCollision->OnComponentEndOverlap.AddDynamic(this, &AActorGreenOnion::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AActorGreenOnion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorGreenOnion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorGreenOnion::AttackStart()
{
	capsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("hello"));
}

void AActorGreenOnion::AttackEnd()
{
	capsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AActorGreenOnion::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("start"));
		}
	}
}

void AActorGreenOnion::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("end"));
	}
	
}



