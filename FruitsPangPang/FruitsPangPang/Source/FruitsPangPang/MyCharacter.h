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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "test");
		class UStaticMeshComponent* collisionTest;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction | Mesh")
	//	class UStaticMeshComponent* GreenOnionMesh;
	//
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction | Mesh")
	//	class UStaticMeshComponent* CarrotMesh;

	UParticleSystemComponent* P_Star;

public:
	USoundBase* dizzySound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
		class USoundCue* LobbyBGM;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
		class USoundCue* InGameBGM;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
		class USoundCue* ResultBGM;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
		class USoundCue* TickTockBGM;
	UPROPERTY()
	class UAudioComponent* SpawnedInGameBGM;

public:
	FTimerHandle TimerHandle;

	UFUNCTION()
		void onTimerEnd();

	UFUNCTION()
		void OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void GreenOnionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void GreenOnionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void CarrotBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void CarrotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


	//���濡 �پ��ִ� ����, ���
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UStaticMeshComponent* GreenOnionBag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UStaticMeshComponent* CarrotBag;

	//speed�� ���� �ִϸ��̼��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "speed")
		float GroundSpeedd;

	float ServerStoreGroundSpeed;
	virtual void Jump() override;

	//fruitid is using sync banana
	void Throw(const FVector& location,FRotator rotation, const int& fruitType, const int& fruitid);
	void ThrowInAIMode(const FVector& location, FRotator rotation, const int& fruitType, const int& fruitid);
	bool bLMBDown;
	void LMBDown();

	UFUNCTION(BlueprintCallable)
		void LMBUp();
public:
	//related HUD
	//UPROPERTY()�� ����ϸ� GC������ (garbage collector) Dangling pointer��  �Ͼ���ʴ´�
	UPROPERTY()
		class UMainWidget* mMainWidget;
	UPROPERTY()
		class UUserWidget* mLoadingWidget;
	UPROPERTY()
		class UUserWidget* mWaitingWidget;
	UPROPERTY()
		class UUserWidget* mLoginWidget;
	UPROPERTY()
		class UGameMatchWidget* mMatchWidget;
	UPROPERTY()
		class UStoreWidget* mStoreWidget;

	class UMainWidget* MakeMainHUD();

	void ShowedInMinimap();
	class UScoreWidget* mScoreWidget;
	

	void MakeLoadingHUD();
	void ShowLoginHUD();
	void ShowMatchHUD();
public:
	//Player Stats
	int GameState;	// -1 = INVALID , 0 = Lobby, 1 = InGame
public:
	//related Lobby
	void InteractNpc();
public:
	//related Network
	bool s_connected;	//server_connected;
	virtual bool ConnServer() override;
	virtual void recvPacket() override;

	virtual bool ConnLobbyServer() override;
	virtual void recvLobbyPacket() override;

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
	//short hp;
	// -- hit event


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
		bool bAttacking;

	bool bIsMoving;
	void Attack();
	virtual void Throw() override;

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
		void GreenOnionAttackStart();
	UFUNCTION(BlueprintCallable)
		void GreenOnionAttackEnd();
	UFUNCTION(BlueprintCallable)
		void CarrotAttackStart();
	UFUNCTION(BlueprintCallable)
		void CarrotAttackEnd();
	UFUNCTION(BlueprintCallable)
		void PickEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* ThrowMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* AnimThrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* SlashMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* AnimSlashMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* StabbingMontage;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* AnimStabbingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* PickSwordMontage;
	void PickSwordAnimation();
	void DropSwordAnimation();


	
public:	

	//Related Shop, Equip Skin
	int Cash;		//���� ���� ��.
	short skinType;	//���� ������ ��Ų Ÿ�� 0 - None , 1 - ... (��Ų ������ ��ȣ)
	unsigned char haveitems[PLAYER_HAVE_ITEM_LOBBY];
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UStaticMeshComponent* SkinParts;
	void EquipSkin();
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
	void ChangeSelectedHotKey(int WannaChange);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
