// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <map>
#include "BaseCharacter.generated.h"

UCLASS()
class FRUITSPANGPANG_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
public:
	//related Mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction | Mesh")
		class UStaticMeshComponent* SM_GreenOnion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction | Mesh")
		class UStaticMeshComponent* SM_Carrot;
public:
	//related DamageEvent

	UFUNCTION()
		virtual void GreenOnionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void GreenOnionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);	
	UFUNCTION()		
		virtual void CarrotBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void CarrotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	std::map<int, class AActor*> DamagedActorCollector;
protected:
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	FString CharacterName;
public:
	UPROPERTY(BlueprintReadOnly)
	int hp;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDie;

	UPROPERTY(BlueprintReadWrite)
	bool bStepBanana;

	UPROPERTY(BlueprintReadWrite)
	bool bHitbyFruit;

public:
	//related inventory

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		int SelectedHotKeySlotNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		int SavedHotKeySlotNum;		//Save HotKey's Slot Num When Attack() Because it will be use for Throww() to get Fruits Path


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		class AInventory* mInventory;

	virtual void ChangeSelectedHotKey(int WannaChange);

public:
	//related throw
	UFUNCTION(BlueprintCallable)
		virtual void Throw();
	//UFUNCTION(BlueprintCallable)
	//	virtual void AttackEnd();

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
	// bIsUndertheTree - Ai???? ???????????? ?? Base?? ??????? - ????
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "tree")
		bool bIsUndertheTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "punnet")
		bool bIsUnderthePunnet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HealSpawner")
		bool bIsUndertheHealSpawner;	

public:
	// 112.152.55.49  127.0.0.1  , 112.153.53.142
	const char* SERVER_ADDR = "127.0.0.1";
public:
	//Game Server
	virtual bool ConnServer();
	SOCKET s_socket;
	SOCKADDR_IN server_addr;
	WSA_OVER_EX recv_expover;
	int		_prev_size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "network")
		int c_id; //received id from server
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "network")
		int overID;	//overlapped I/O use this
	virtual void recvPacket();
public:
	//Lobby Server
	virtual bool ConnLobbyServer();
	SOCKET l_socket;	//lobby socket
	SOCKADDR_IN l_server_addr;
	WSA_OVER_EX l_recv_expover;
	int		l_prev_size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "network")
		int l_c_id; //received id from server
	virtual void recvLobbyPacket();


};
