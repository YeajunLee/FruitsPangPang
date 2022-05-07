// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "MyCharacter.h"
#include "BaseCharacter.h"
#include "Network.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
	:BombOwner(nullptr)
	,_fType(0)
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
			Network::GetNetwork()->send_pos_packet(BombOwner->s_socket,pos.X, pos.Y, pos.Z, POS_TYPE_DURIAN);
			//send packet
		}
		//Ai전용 패킷이 없음.
	}
}

void AProjectile::ApplyDamageCPP(AActor* otherActor)
{

	auto p = Cast<ABaseCharacter>(otherActor);

	//UE_LOG(LogTemp, Log, TEXT("ApplyDamage : in apply damage"));
	if (nullptr != p)
	{
		//UE_LOG(LogTemp, Log, TEXT("ApplyDamage : ready damage"));
		//p->TakeDamage(0, FDamageEvent(), nullptr, this);
		UGameplayStatics::ApplyDamage(otherActor, 1, GetInstigatorController(), this, NULL);
	}
}

void AProjectile::SyncBananaTransform()
{
	if (BombOwner != nullptr)
	{
		if (BombOwner->GetController()->IsPlayerController())
		{
			Network::GetNetwork()->send_sync_banana(BombOwner->s_socket, GetActorLocation(), GetActorRotation(), uniqueID);
			//send packet
		}
		//Ai전용 패킷이 없음.
	}
}
