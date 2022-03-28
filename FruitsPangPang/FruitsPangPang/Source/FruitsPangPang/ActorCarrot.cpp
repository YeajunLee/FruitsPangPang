// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorCarrot.h"
#include "MyCharacter.h"



// Sets default values
AActorCarrot::AActorCarrot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("static mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CarrotAsset(TEXT("/Game/Assets/Fruits/Carrot/SM_Carrot.SM_Carrot"));
	if (CarrotAsset.Succeeded())
		staticMeshComponent->SetStaticMesh(CarrotAsset.Object);

	capsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("collision"));
	capsuleCollision->SetupAttachment(staticMeshComponent);

	capsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	capsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &AActorCarrot::OnOverlapBegin);
	capsuleCollision->OnComponentEndOverlap.AddDynamic(this, &AActorCarrot::OnOverlapEnd);
}


// Called when the game starts or when spawned
void AActorCarrot::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AActorCarrot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AActorCarrot::AttackStart()
{
	capsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AActorCarrot::AttackEnd()
{
	capsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AActorCarrot::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (GEngine)
		{
			auto p = Cast<AMyCharacter>(OtherActor);
			if (nullptr != p)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("start"));
		}
	}
}

void AActorCarrot::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("end"));
	}
}

