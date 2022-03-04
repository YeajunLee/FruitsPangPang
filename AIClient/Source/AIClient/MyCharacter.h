// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class AICLIENT_API AMyCharacter : public ACharacter
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

	//speed에 따른 애니메이션을 위해
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "speed")
	float GroundSpeed;

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	void Throw();

	void Throw(const FVector& location, FRotator rotation, const FName& path);
	bool bLMBDown;
	void LMBDown();

	UFUNCTION(BlueprintCallable)
	void LMBUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	int c_id;
	bool s_connected;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* ThrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* AnimThrowMontage;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	//	class AInventory* mInventory;

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

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
