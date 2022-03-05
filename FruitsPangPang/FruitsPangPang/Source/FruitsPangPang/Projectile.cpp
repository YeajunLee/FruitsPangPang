// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "MyCharacter.h"
#include "Network.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile()
	:BombOwner(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;
	ProjectileMovementComponent->Bounciness = 0.6f;
	ProjectileMovementComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::durianPacket(const FVector& pos)
{
	if (BombOwner != nullptr)
	{
		if (BombOwner->GetController()->IsPlayerController())
		{
			Network::GetNetwork()->send_pos_packet(pos.X, pos.Y, pos.Z, POS_TYPE_DURIAN);
			//send packet
		}
	}
}