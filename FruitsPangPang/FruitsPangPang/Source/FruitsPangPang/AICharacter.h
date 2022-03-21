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

public:
	//related interact

	virtual void GetFruits() override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
	//	bool OverlapInteract;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
	//	int OverlapInteractId;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact")
	//	bool OverlapType;	//true == Tree , false == Punnet
public:
	//related network

	virtual bool ConnServer() override;
	//const char* SERVER_ADDR = "127.0.0.1";
	//const short SERVER_PORT = 4000;
	//SOCKET s_socket;
	//SOCKADDR_IN server_addr;
	//WSA_OVER_EX recv_expover;
	//int		_prev_size;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "network")
	//	int c_id; //received id from server
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "network")
	//	int overID;	//overlapped I/O use this
	virtual void recvPacket() override;
//public:
//	int hp;
//public:
//	//related inventory
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
//		int SelectedHotKeySlotNum;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
//		int SavedHotKeyItemCode;		//Save HotKey's ItemCode When Attack() Because it will be use for Throww() to get Fruits Path
//
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
//		class AInventory* mInventory;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
