// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "ActorGreenOnion.generated.h"

UCLASS()
class FRUITSPANGPANG_API AActorGreenOnion : public AActor
{

	GENERATED_BODY()


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* staticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCapsuleComponent* capsuleCollision;

	UFUNCTION(BlueprintCallable)
		void AttackStart();
	UFUNCTION(BlueprintCallable)
		void AttackEnd();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Sets default values for this actor's properties
	AActorGreenOnion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

};
