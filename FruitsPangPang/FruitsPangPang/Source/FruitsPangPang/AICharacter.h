// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "BaseCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

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

	UCharacterMovementComponent* movement;
	//UFUNCTION(BlueprintCallable)
	//	void Throw_AI();
public:
	// related mesh
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction | Mesh")
	//	class UStaticMeshComponent* GreenOnionMesh;
	//
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction | Mesh")
	//	class UStaticMeshComponent* CarrotMesh;


	UParticleSystemComponent* P_Star1;
	FTimerHandle TimerHandle;

	USoundBase* dizzySound1;

	UFUNCTION()
		void OnTimeEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "collision")
		class UStaticMeshComponent* collisionBox;

	UFUNCTION()
		void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
public:
	//related interact
	
	virtual	void GetFruits() override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
	//	bool OverlapInteract;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
	//	int OverlapInteractId;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact")
	//	bool OverlapType;	//true == Tree , false == Punnet
protected:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	//related network

	virtual bool ConnServer() override;
	virtual void recvPacket() override;
public:
	//int hp;
public:
	//related inventory

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	//	int SelectedHotKeySlotNum;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	//	int SavedHotKeyItemCode;		//Save HotKey's ItemCode When Attack() Because it will be use for Throww() to get Fruits Path


	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	//	class AInventory* mInventory;


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "tree")
		class ATree* mTree;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "tree")
	//	bool bIsUndertheTree;

public:
	//related attack
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anims")
		bool bAttacking;

	void Attack();

	virtual void Throw() override;

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
