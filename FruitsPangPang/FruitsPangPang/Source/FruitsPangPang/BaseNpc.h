// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseNpc.generated.h"

UCLASS()
class FRUITSPANGPANG_API ABaseNpc : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseNpc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		void OverlapNpcBoxBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;

	UFUNCTION()
		void OverlapNpcBoxEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) ;
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Npc | Mesh")
		class UStaticMeshComponent* mStaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Npc | Coliision")
		class UBoxComponent* mBoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Npc")
		int NpcCode;

};
