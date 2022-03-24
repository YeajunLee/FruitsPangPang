// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);


UCLASS()
class AICLIENT_API AAICharacter : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AAICharacter();

	//speed에 따른 애니메이션을 위해
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "speed")
	float GroundSpeed_AI;

	UFUNCTION(BlueprintCallable)
	void Throw_AI();

public:
	//related interact
	UFUNCTION(BlueprintCallable)
		void GetFruits();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
		bool OverlapInteract;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
		int OverlapInteractId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact")
		bool OverlapType;	//true == Tree , false == Punnet
public:
	//related network

	bool ConnServer();
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
	void recvPacket();
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "tree")
	class ATree* mTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "tree")
	bool bIsUndertheTree;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	//UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Delegate")
	FOnAttackEndDelegate OnAttackEnd;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UAnimInstance* AnimInstance;

	//UFUNCTION(BlueprintCallable)
	//void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* ThrowMontage_AI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* AnimThrowMontage_AI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//virtual void PostInitializeComponents() override;
};
