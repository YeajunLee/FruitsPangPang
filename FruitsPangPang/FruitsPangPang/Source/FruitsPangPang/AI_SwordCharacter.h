// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "BaseCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <vector>
#include "AI_SwordCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

//class PunnetInfo {
//public:
//	class APunnet* mPunnet;
//	bool bIgnored;
//	PunnetInfo();
//	PunnetInfo(class APunnet* punnet);
//};

UCLASS()
class FRUITSPANGPANG_API AAI_SwordCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAI_SwordCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "speed")
		float GroundSpeed_AI;

public:
	FTimerHandle TimerHandle;

	UFUNCTION()
		void OnTimeEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "collision")
	class UBoxComponent* BananaCollision;
	UFUNCTION()
	void OnBananaBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void GreenOnionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void GreenOnionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void CarrotBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void CarrotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

public:
	//related interact

	virtual	void GetFruits() override;

	/*float ClosestPunnetDistance;
	int TargetPunnetIndex;
	std::vector<PunnetInfo> punnets;*/

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	//related network

	virtual bool ConnServer() override;
	virtual void recvPacket() override;

public:
	//related attack
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anims")
	bool bAttacking;

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
		void GreenOnionAttackStart();
	UFUNCTION(BlueprintCallable)
		void GreenOnionAttackEnd();
	UFUNCTION(BlueprintCallable)
		void CarrotAttackStart();
	UFUNCTION(BlueprintCallable)
		void CarrotAttackEnd();

	FOnAttackEndDelegate OnAttackEnd;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UAnimInstance* AnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* SlashMontage_AI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* StabMontage_AI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
