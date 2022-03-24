// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class FRUITSPANGPANG_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void durianPacket(const FVector& pos);

	UFUNCTION(BlueprintCallable)
		void ApplyDamageCPP(AActor* otherActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		class ABaseCharacter* BombOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		int _fType;

	UPROPERTY(VisibleAnywhere, Category = "default")
		class UProjectileMovementComponent* ProjectileMovementComponent;
};
