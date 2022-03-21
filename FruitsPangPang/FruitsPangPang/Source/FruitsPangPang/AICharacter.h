// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "BaseCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

UCLASS()
class FRUITSPANGPANG_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

	//speed에 따른 애니메이션을 위해
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "speed")
	float GroundSpeed_AI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:
	//related interact
	virtual void GetFruits() override;

public:
	//related network
	virtual bool ConnServer() override;
	virtual void recvPacket() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
