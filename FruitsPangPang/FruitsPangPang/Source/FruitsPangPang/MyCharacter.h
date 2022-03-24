// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Network.h"
#include "BaseCharacter.h"
#include <memory>
#include "../../../Protocol/protocol.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"



UCLASS()
class FRUITSPANGPANG_API AMyCharacter : public ABaseCharacter
{
	GENERATED_BODY()


public:


	// Sets default values for this character's properties
	AMyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Base turn rates to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	//대파 staticmesh component 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default)
		class UStaticMeshComponent* GreenOnionComponent;
	//당근 staticmesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default)
		class UStaticMeshComponent* CarrotComponent;

	//speed에 따른 애니메이션을 위해
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "speed")
		float GroundSpeedd;

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
		void Throww();
	void Throw(const FVector& location, FRotator rotation, const FName& path);
	bool bLMBDown;
	void LMBDown();

	UFUNCTION(BlueprintCallable)
		void LMBUp();
public:
	//related Network
	bool s_connected;	//server_connected;
	virtual bool ConnServer() override;	
	virtual void recvPacket() override;

public:
	// related interact
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact")
		bool bInteractDown;

	void InteractDown();

	void InteractUp();

	virtual void GetFruits() override;

protected:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	UFUNCTION(BlueprintCallable)
		void SendHitPacket();

	//short hp;
	// -- hit event


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
		bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
		void AttackEnd();
	UFUNCTION(BlueprintCallable)
		void SwordAttackStart();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* ThrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* AnimThrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* SlashMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* StabbingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* PickSwordMontage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for forwards/backwards input */
	void MoveForward(float value);
	/** Called for side to side input */
	void MoveRight(float value);
	void TurnAtRate(float rate);
	void LookUpAtRate(float rate);

	UFUNCTION(BlueprintCallable)
	void AnyKeyPressed(FKey Key);


	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
