// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class FRUITSPANGPANG_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	int hp;
public:
	//related inventory

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
		int SelectedHotKeySlotNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
		int SavedHotKeyItemCode;		//Save HotKey's ItemCode When Attack() Because it will be use for Throww() to get Fruits Path


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class AInventory* mInventory;
public:
	//related interact
	UFUNCTION(BlueprintCallable)
		virtual void GetFruits();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
		bool OverlapInteract;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
		int OverlapInteractId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact")
		bool OverlapType;	//true == Tree , false == Punnet
public:
	//related score
	int killcount;
	int deathcount;
	int score;
public:
	// ai <-> tree collision check
	// Ai인데 왜 Base에 있나요? - 수민
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "tree")
		bool bIsUndertheTree;

public:
	virtual bool ConnServer();
	const char* SERVER_ADDR = "127.0.0.1";
	const short SERVER_PORT = 4000;
	SOCKET s_socket;
	SOCKADDR_IN server_addr;
	WSA_OVER_EX recv_expover;
	int		_prev_size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "network")
		int c_id; //received id from server
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "network")
		int overID;	//overlapped I/O use this
	virtual void recvPacket();
};
