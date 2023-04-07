// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Tree.h"
#include "Punnet.h"
#include "Inventory.h"
#include "InventorySlotWidget.h"
#include "MainWidget.h"
#include "MessageBoxWidget.h"
#include "ChatWidget.h"
#include "Projectile.h"
#include "RespawnWindowWidget.h"
#include "RespawnWidget.h"
#include "GameMatchWidget.h"
#include "StoreWidget.h"
#include "Particles/ParticleSystemComponent.h "
#include "Sound/SoundBase.h"
#include "Sound/SoundCue.h"
#include "PointOfInterestComponent.h"
#include "ScoreWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AMyCharacter::AMyCharacter()
	:s_connected(false)
	, bInteractDown(false)
	, bIsMoving(false)
	, ServerStoreGroundSpeed(0)
	, GameState(-1)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeLocation(FVector(-70.f, 0.f, 0.f));
	FollowCamera->bUsePawnControlRotation = false;

	//Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//GroundSpeed = 0.f;

	//Player Stats Initialize------------------------------------------------------------------------------



	//-----------------------------------------------------------------------------------------------------
	
	//2022 - 07 - 06 OrientRotationToMoveMent�� true�� �ϸ� �ڷΰ��� �������� �����ȵǴ� ���װ� �����.
	//������ �ɰ��غ��̸� ���� ����� ���� ã�ƺ���.
	//�� ������ ��~�� �� �ʹݺ��� �־��� ���������� ���������� ���ϴٰ� �̹��� �̽��� �Ǿ� ã�� ����. 
	GetCharacterMovement()->bOrientRotationToMovement = false; //ĳ���� ���⿡ ���� 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1080.f, 0.f); // ȸ��
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GreenOnionBag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenOnionBag"));
	GreenOnionBag->SetupAttachment(GetMesh());

	CarrotBag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrotBag"));
	CarrotBag->SetupAttachment(GetMesh());
	
	SkinParts = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkinParts"));
	SkinParts->SetupAttachment(GetMesh());

	collisionTest = CreateDefaultSubobject < UStaticMeshComponent>(TEXT("Test"));
	collisionTest->SetupAttachment(GetRootComponent());
	collisionTest->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnCapsuleOverlapBegin);
	
	
	P_Star = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("StarParticle"));
	P_Star->SetupAttachment(RootComponent);
	P_Star->bAutoActivate = false;
	P_Star->SetRelativeLocation( FVector(50.f, 10.f, 10.f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Assets/Fruits/Banana/P_Stars.P_Stars"));
	if (ParticleAsset.Succeeded())
	{
		P_Star->SetTemplate(ParticleAsset.Object);
	}

	
	static ConstructorHelpers::FObjectFinder<USoundBase> dizzySoundAsset(TEXT("/Game/Assets/Fruits/Banana/S_dizzy.S_dizzy"));
	if (dizzySoundAsset.Succeeded())
	{
		dizzySound = dizzySoundAsset.Object;
	}
	
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{

	Super::BeginPlay();
	Today = FDateTime::Now();
	GreenOnionBag->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GreenOnionBag->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GreenOnionBag"));
	CarrotBag->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarrotBag->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("CarrotBag"));
	SkinParts->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GreenOnionBag->SetHiddenInGame(true, false);
	CarrotBag->SetHiddenInGame(true, false);
	SkinParts->SetHiddenInGame(true);

	if (nullptr == GetController())
	{
		UE_LOG(LogTemp, Error, TEXT("Not controller!! PleaseMake SpawnDefaultController and AutoPossessPlayer EAutoReceiveInput::Disabled"));
		return;
	}
	if (GetController()->IsPlayerController())
	{
		Network::GetNetwork()->mMyCharacter = this;		
		/*
			Setting Actor Params Before SpawnActor's BeginPlay
		*/
		FName path = TEXT("Blueprint'/Game/Inventory/Inventory_BP.Inventory_BP_C'"); //_C�� �� �ٿ��� �ȴٰ� ��.
		UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		FTransform spawnLocAndRot{ GetActorLocation() };
		mInventory = GetWorld()->SpawnActorDeferred<AInventory>(GeneratedInventoryBP, spawnLocAndRot);
		mInventory->mOwnerCharacter = this;	// ExposeOnSpawn�ϰ� SpawnActor���� �� �־��ִ°� C++�� ¥�� �̷��� �ε�
		mInventory->mAmountOfSlots = 5;
		mInventory->FinishSpawning(spawnLocAndRot);

		//������ ���� ���Ǽ��� ���� ���� �־�����, ���߿��� �������� ��Ŷ�� �޾��� �� ������� ������.
		MakeMainHUD();

		FItemInfo itemClass;
		itemClass.ItemCode = 1;	//�丶�� 30�� ����
		itemClass.IndexOfHotKeySlot = 0;
		itemClass.Name = AInventory::ItemCodeToItemName(itemClass.ItemCode);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(itemClass.ItemCode);

		mInventory->UpdateInventorySlot(itemClass, 100);

		itemClass.ItemCode = 4;	//���� 30�� ����
		itemClass.IndexOfHotKeySlot = 1;
		itemClass.Name = AInventory::ItemCodeToItemName(itemClass.ItemCode);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(itemClass.ItemCode);
		mInventory->UpdateInventorySlot(itemClass, 100);

		itemClass.ItemCode = 7; //���� 1�� ����
		itemClass.IndexOfHotKeySlot = 2;
		itemClass.Name = AInventory::ItemCodeToItemName(itemClass.ItemCode);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(itemClass.ItemCode);
		mInventory->UpdateInventorySlot(itemClass, 1);


		itemClass.ItemCode = 9;	//�θ��� 30�� ����
		itemClass.IndexOfHotKeySlot = 3;
		itemClass.Name = AInventory::ItemCodeToItemName(itemClass.ItemCode);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(itemClass.ItemCode);
		mInventory->UpdateInventorySlot(itemClass, 30);

		itemClass.ItemCode = 11; //�ٳ��� 1�� ����
		itemClass.IndexOfHotKeySlot = 4;
		itemClass.Name = AInventory::ItemCodeToItemName(itemClass.ItemCode);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(itemClass.ItemCode);
		mInventory->UpdateInventorySlot(itemClass, 200);

	}
	else {
		GetCharacterMovement()->BrakingDecelerationWalking = 0;
		GetCharacterMovement()->BrakingFrictionFactor = 0;
		GetCharacterMovement()->GravityScale = 0.0;
	}
}

void AMyCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	if (mInventory)
	{
		mInventory->Destroy();
		mInventory = nullptr;
	}

	SyncInAirDelegate.Clear();
	closesocket(l_socket);
	closesocket(s_socket);
	Network::GetNetwork()->release();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		float PitchClamp = FMath::ClampAngle(Rotation.Pitch, -45.f, 30.f);
		FRotator RotationControl(PitchClamp, Rotation.Yaw, Rotation.Roll);

		if (GetController()->IsPlayerController()) {
			SleepEx(0, true);
			auto pos = GetTransform().GetLocation();
			auto rot = GetTransform().GetRotation();

			ServerSyncElapsedTime += DeltaTime;
			if (ServerSyncDeltaTime < ServerSyncElapsedTime)
			{
				if (!bIsDie)
					send_move_packet(s_socket,GetCharacterMovement()->IsFalling(), pos.X, pos.Y, pos.Z, rot, GroundSpeedd,GetCharacterMovement()->Velocity);
				ServerSyncElapsedTime = 0.0f;
			}
		
			float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();
			GroundSpeedd = CharXYVelocity;

			if (Today.GetDay() != FDateTime::Now().GetDay())
			{
				if (nullptr != Network::GetNetwork()->mMyCharacter)
				{
					send_daypass_packet(Network::GetNetwork()->mMyCharacter->l_socket);
					Today = FDateTime::Now();
				}
			}
		}
		else {
			//Update GroundSpeedd (22-04-05)
			GroundSpeedd = ServerStoreGroundSpeed;
			//Update Interpolation (22-11-25)
			GetCharacterMovement()->Velocity = CharMovingSpeed;
		}
		
		auto a = GetTransform().GetLocation().Y;
		Controller->SetControlRotation(RotationControl);		
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); //�÷��̾� �Է� ���� ��Ұ� ��ȿ���� Ȯ���ϴ� ��ũ��

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMyCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMyCharacter::LMBUp);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyCharacter::InteractDown);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMyCharacter::InteractUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::AddControllerPitchInput);


	//PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);
}

void AMyCharacter::ChangeSelectedHotKey(int WannaChange)
{
	int tmp = SelectedHotKeySlotNum;
	SelectedHotKeySlotNum = WannaChange;
	if (tmp != SelectedHotKeySlotNum)
	{
		mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
		mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
		//send_change_hotkeyslot_packet(s_socket, SelectedHotKeySlotNum);
	}
}

void AMyCharacter::AnyKeyPressed(FKey Key)
{
	if (bAttacking) return;
	if (Key == EKeys::Enter)
	{
		if (0 == GameState)
		{
			if (mMainWidget)
				if (mMainWidget->W_Chat)
					mMainWidget->W_Chat->ControlChat();
		}
	}
	else if (Key == EKeys::One)
	{
		//UE_LOG(LogTemp, Log, TEXT("One Hitted"));
		DropSwordAnimation();
		ChangeSelectedHotKey(0);		
	}
	else if (Key == EKeys::Two)
	{
		//UE_LOG(LogTemp, Log, TEXT("two Hitted"));
		if (mInventory->IsSlotValid(2) && SelectedHotKeySlotNum == 2)
		{
			DropSwordAnimation();
		}
		DropSwordAnimation();
		ChangeSelectedHotKey(1);
		
	}
	else if (Key == EKeys::Three)
	{
		//UE_LOG(LogTemp, Log, TEXT("three Hitted"));
		ChangeSelectedHotKey(2);
		if (mInventory->IsSlotValid(SelectedHotKeySlotNum))
		{
			PickSwordAnimation();
		}

	}
	else if (Key == EKeys::Four)
	{
		//UE_LOG(LogTemp, Log, TEXT("Four Hitted"));
		if (mInventory->IsSlotValid(2) && SelectedHotKeySlotNum==2)
		{
			DropSwordAnimation();
		}
		DropSwordAnimation();
		ChangeSelectedHotKey(3);
		
	}
	else if (Key == EKeys::Five)
	{
		//UE_LOG(LogTemp, Log, TEXT("Five Hitted"));
		DropSwordAnimation();
		ChangeSelectedHotKey(4);
	}

	else if (Key == EKeys::MouseScrollUp)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		//SavedHotKeyItemCode = mInventory->mSlots[SelectedHotKeySlotNum].ItemClass.ItemCode;

		//UE_LOG(LogTemp, Log, TEXT("Wheel Down"));
		int tmp = SelectedHotKeySlotNum;
		DropSwordAnimation();
		SelectedHotKeySlotNum = max(SelectedHotKeySlotNum - 1, 0);
		if (tmp != SelectedHotKeySlotNum)
		{
			mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
			mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
			
			if (SelectedHotKeySlotNum == 2 && mInventory->IsSlotValid(SelectedHotKeySlotNum))
			{
				PickSwordAnimation();
			}
			if (SelectedHotKeySlotNum == 1 && mInventory->IsSlotValid(2))
			{
				DropSwordAnimation();
			}
		}
	}
	else if (Key == EKeys::MouseScrollDown)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		//UE_LOG(LogTemp, Log, TEXT("Wheel Up"));
		int tmp = SelectedHotKeySlotNum;
		DropSwordAnimation();
		SelectedHotKeySlotNum = min(SelectedHotKeySlotNum + 1, 4);
		if (tmp != SelectedHotKeySlotNum)
		{
			mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
			mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
			if (SelectedHotKeySlotNum == 2 && mInventory->IsSlotValid(2))
			{
				PickSwordAnimation();
			}
			if (SelectedHotKeySlotNum == 3 && mInventory->IsSlotValid(2))
			{
				DropSwordAnimation();

			}
			
		}
	}
	else if (Key == EKeys::P)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GAMETIME);
	}
	else if (Key == EKeys::Six)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 1);
	}
	else if (Key == EKeys::Seven)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 2);
	}
	else if (Key == EKeys::Eight)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 3);
	}
	else if (Key == EKeys::Y)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 4);
	}
	else if (Key == EKeys::U)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 5);
	}
	else if (Key == EKeys::I)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 6);
	}
	else if (Key == EKeys::H)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 7);
	}
	else if (Key == EKeys::J)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 8);
	}
	else if (Key == EKeys::N)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 9);
	}
	else if (Key == EKeys::M)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 10);
	}
	else if (Key == EKeys::Comma)
	{
		send_Cheat(s_socket, CHEAT_TYPE_GIVEITEM, 11);
	}
}

void AMyCharacter::MoveForward(float value)
{
	if (Controller != nullptr && value != 0.f)
	{

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);

	}
}

void AMyCharacter::MoveRight(float value)
{
	if (Controller != nullptr && value != 0.f)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}

void AMyCharacter::TurnAtRate(float rate)
{
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::LookUpAtRate(float rate)
{
	AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::InteractDown()
{
	if (OverlapInteract)
	{
		switch (GameState)
		{
		case 0:
			InteractNpc();
			break;
		case 1:
			GetFruits();
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Player's GameState is: '%d' UnExpected Value !!"),GameState);
			break;
		}	
		bInteractDown = true;
	}

	
}

void AMyCharacter::InteractUp()
{
	
	if (OverlapInteract)
	{
		PickSwordAnimation();
		if (mInventory->mSlots[2].ItemClass.ItemCode == 7 && SM_GreenOnion->bHiddenInGame)
		{
			GreenOnionBag->SetHiddenInGame(false, false);
			CarrotBag->SetHiddenInGame(true, false);
		}
		if (mInventory->mSlots[2].ItemClass.ItemCode == 8 && SM_Carrot->bHiddenInGame)
		{
			GreenOnionBag->SetHiddenInGame(true, false);
			CarrotBag->SetHiddenInGame(false, false);
		}
	}
	bInteractDown = false;
}


void AMyCharacter::GetFruits()
{
	Super::GetFruits();
	if (OverlapType)
	{
		if (OverlapInteractId != -1)
		{
			Network::GetNetwork()->mTree[OverlapInteractId]->CanHarvest = false;
			send_getfruits_tree_packet(s_socket, OverlapInteractId);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Overlap is -1 But Try GetFruits - Type = Tree"));
		}
	}
	else {
		if (OverlapInteractId != -1)
		{
			Network::GetNetwork()->mPunnet[OverlapInteractId]->CanHarvest = false;
			send_getfruits_punnet_packet(s_socket, OverlapInteractId);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Overlap is -1 But Try GetFruits - Type = Punnet"));
		}
	}
}


void AMyCharacter::InteractNpc()
{
	switch (OverlapInteractId)
	{
	case 0:
		ShowMatchHUD();
		break;
	case 1:
		mMainWidget->ShowStoreWidget();
		break;
	break;
	}
}

void AMyCharacter::LMBDown()
{
	bLMBDown = true;
	Attack();
}

void AMyCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMyCharacter::Jump()
{
	Super::Jump();
	switch (GameState)
	{
	case 1:
		if (GetController()->IsPlayerController()) {
		send_anim_packet(s_socket, Network::AnimType::Jump);
		}
		break;
	}


}

void AMyCharacter::Attack()
{
	if (!bAttacking)
	{
		if (mInventory->mSlots[SelectedHotKeySlotNum].Amount > 0)
		{
			SavedHotKeySlotNum = SelectedHotKeySlotNum;
			int HotKeyItemCode = mInventory->mSlots[SelectedHotKeySlotNum].ItemClass.ItemCode;
			bAttacking = true;

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			
			if (HotKeyItemCode == 7)
			{	
				if (AnimInstance && SlashMontage)
				{
					AnimInstance->Montage_Play(SlashMontage, 1.5f);
					AnimInstance->Montage_JumpToSection(FName("Default"), SlashMontage);
					send_anim_packet(s_socket, Network::AnimType::Slash);
					
				}
			}
			else if (HotKeyItemCode == 8)
			{
				if (AnimInstance && StabbingMontage)
				{
					AnimInstance->Montage_Play(StabbingMontage, 1.2f);
					AnimInstance->Montage_JumpToSection(FName("Default"), StabbingMontage);
					send_anim_packet(s_socket, Network::AnimType::Stab);
					
				}
			}
			else if (AnimInstance && ThrowMontage)
			{
				AnimInstance->Montage_Play(ThrowMontage, 2.f);
				AnimInstance->Montage_JumpToSection(FName("Default"), ThrowMontage);
				send_anim_packet(s_socket, Network::AnimType::Throw);
				//send_useitem_packet(s_socket, SelectedHotKeySlotNum, 1);
			}
			
		}
	}
}

void AMyCharacter::AttackEnd()
{
	bAttacking = false;
	if (bLMBDown && (bStepBanana == false))
	{
		Attack();
	}
}

void AMyCharacter::onTimerEnd()
{
	EnableInput(Cast<APlayerController>(this));
	bStepBanana = false;
}

void AMyCharacter::OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	

	if (OtherActor && (OtherActor != this) && OtherActor)
	{
		auto banana = Cast<AProjectile>(OtherActor);
		if (nullptr != banana)
		{
			if (banana->_fType == 11)
			{
				if (0 == banana->BananaJudging)
				{
					banana->BananaJudging = 1;
					send_step_banana_packet(s_socket, banana->uniqueID);
				}				
			}
		}
	}
}

void AMyCharacter::StepBanana()
{
	bStepBanana = true;
	DisableInput(Cast<APlayerController>(this));
	if (P_Star && P_Star->Template)
	{
		P_Star->ToggleActive();
	}
	UGameplayStatics::PlaySoundAtLocation(this, dizzySound, GetActorLocation());
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyCharacter::onTimerEnd, 2.5, false);
}

void AMyCharacter::GreenOnionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::GreenOnionBeginOverlap(OverlappedComp,OtherActor,OtherComp,OtherBodyIndex, bFromSweep,SweepResult);
	//UE_LOG(LogTemp, Log, TEXT("%s"), *UKismetSystemLibrary::GetDisplayName(OtherComp));
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (GEngine)
		{
			auto victim = Cast<ABaseCharacter>(OtherActor);
			if (nullptr != victim)
			{
				DamagedActorCollector.insert({ victim->c_id, victim });
				//TSubclassOf<UDamageType> dmgCauser;
				//dmgCauser = UDamageType::StaticClass();
				//
				//if (!this->SM_GreenOnion->bHiddenInGame)
				//{
				//	//������ ���ذ��� �ɼ�������, ������� ������ �� �Ը���� fruitType�� �����ֵ��� �Ѵ�.
				//	dmgCauser.GetDefaultObject()->DamageFalloff = 7.0f;
				//}
				//if (!this->SM_Carrot->bHiddenInGame)
				//{
				//	dmgCauser.GetDefaultObject()->DamageFalloff = 8.0f;
				//}				
				//UGameplayStatics::ApplyDamage(OtherActor, 1, GetInstigatorController(), this, dmgCauser);
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("start :"));
				//UE_LOG(LogTemp, Log, TEXT("Damage Type %d"), dmgCauser.GetDefaultObject()->DamageFalloff);
			}
		}
	}
}

void AMyCharacter::GreenOnionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::GreenOnionEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}

void AMyCharacter::CarrotBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::GreenOnionBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (GEngine)
		{
			auto victim = Cast<ABaseCharacter>(OtherActor);
			if (nullptr != victim)
			{
				DamagedActorCollector.insert({ victim->c_id, victim });
			}
		}
	}
}

void AMyCharacter::CarrotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AMyCharacter::GreenOnionAttackStart()
{
	SM_GreenOnion->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMyCharacter::GreenOnionAttackEnd()
{
	for (auto& p : DamagedActorCollector)
	{
		auto victim = p.second;
		TSubclassOf<UDamageType> dmgCauser;
		dmgCauser = UDamageType::StaticClass();
		dmgCauser.GetDefaultObject()->DamageFalloff = 0.0f;

		if (!this->SM_GreenOnion->bHiddenInGame)
		{
			//������ ���ذ��� �ɼ�������, ������� ������ �� �Ը���� fruitType�� �����ֵ��� �Ѵ�.
			dmgCauser.GetDefaultObject()->DamageFalloff = 7.0f;
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Daepa is not Equipped %d"), c_id);

		UGameplayStatics::ApplyDamage(victim, 1, GetInstigatorController(), this, dmgCauser);
	}
	DamagedActorCollector.clear();
	SM_GreenOnion->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMyCharacter::CarrotAttackStart()
{
	SM_Carrot->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMyCharacter::CarrotAttackEnd()
{
	for (auto& p : DamagedActorCollector)
	{
		auto victim = p.second;
		TSubclassOf<UDamageType> dmgCauser;
		dmgCauser = UDamageType::StaticClass();
		dmgCauser.GetDefaultObject()->DamageFalloff = 0.0f;

		if (!this->SM_Carrot->bHiddenInGame)
		{
			dmgCauser.GetDefaultObject()->DamageFalloff = 8.0f;
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Dangeun is not Equipped %d"), c_id);
		UGameplayStatics::ApplyDamage(victim, 1, GetInstigatorController(), this, dmgCauser);	
	}
	DamagedActorCollector.clear();
	SM_Carrot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AMyCharacter::PickEnd()
{
	bAttacking = false;
}

void AMyCharacter::PickSwordAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && PickSwordMontage)
	{
		FItemInfo info;
		bool isempty;
		int amount;
		mInventory->GetItemInfoAtSlotIndex(SelectedHotKeySlotNum, isempty, info, amount);
		switch (info.ItemCode)
		{
		case 7:
			SM_GreenOnion->SetHiddenInGame(false, false);
			GreenOnionBag->SetHiddenInGame(true, false);
			SM_Carrot->SetHiddenInGame(true, false);
			CarrotBag->SetHiddenInGame(true, false);
			send_anim_packet(s_socket, Network::AnimType::PickSword_GreenOnion);
			break;
		case 8:
			SM_Carrot->SetHiddenInGame(false, false);
			CarrotBag->SetHiddenInGame(true, false);
			SM_GreenOnion->SetHiddenInGame(true, false);
			GreenOnionBag->SetHiddenInGame(true, false);
			send_anim_packet(s_socket, Network::AnimType::PickSword_Carrot);
			break;
		}		
		AnimInstance->Montage_Play(PickSwordMontage, 1.5f);
		AnimInstance->Montage_JumpToSection(FName("Default"), PickSwordMontage);

	}
}
void AMyCharacter::DropSwordAnimation()
{
	if (2 != SelectedHotKeySlotNum) return;
	if (!mInventory->IsSlotValid(2)) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && PickSwordMontage)
	{
		FItemInfo info;
		bool isempty;
		int amount;
		mInventory->GetItemInfoAtSlotIndex(SelectedHotKeySlotNum, isempty, info, amount);
		switch (info.ItemCode)
		{
		case 7:
			SM_GreenOnion->SetHiddenInGame(true, false);
			GreenOnionBag->SetHiddenInGame(false, false);
			SM_Carrot->SetHiddenInGame(true, false);
			CarrotBag->SetHiddenInGame(true, false);
			send_anim_packet(s_socket, Network::AnimType::PickSword_GreenOnion);
			break;
		case 8:
			SM_Carrot->SetHiddenInGame(true, false);
			CarrotBag->SetHiddenInGame(false, false);
			SM_GreenOnion->SetHiddenInGame(true, false);
			GreenOnionBag->SetHiddenInGame(true, false);
			send_anim_packet(s_socket, Network::AnimType::PickSword_Carrot);
			break;
		}
		AnimInstance->Montage_Play(PickSwordMontage, 1.5f);
		AnimInstance->Montage_JumpToSection(FName("Default"), PickSwordMontage);
	}

	send_anim_packet(s_socket, Network::AnimType::DropSword);
}

// ������ 3�� �������� ȭ�鿡 1,2,3�� �������� ��Ÿ���� �ʰ� �ϵ��� �ϴ� �Լ�
//void AMyCharacter::ShowedInMinimap()
//{
//	
//	if (mInventory->mMainWidget->mScoreWidget != nullptr)
//	{
//		if (mInventory->mMainWidget->mScoreWidget->ScoreBoard[0].GetCharacter()->c_id == this->c_id)
//		{
//			POIcomponent->isOn = false;
//		}
//		else
//			POIcomponent->isOn = true;
//	}
//}

void AMyCharacter::Throw()
{
	if (SavedHotKeySlotNum == 4)
	{
		//�ٳ����� �������� ������ ������ spawn �ǵ��� ���� �߽��ϴ� - ����
		FTransform SocketTransform = GetMesh()->GetSocketTransform("BananaSocket");
		FRotator CameraRotate = FollowCamera->GetComponentRotation();
		FTransform trans(CameraRotate.Quaternion(), SocketTransform.GetLocation());
		int HotKeyItemCode = mInventory->mSlots[SavedHotKeySlotNum].ItemClass.ItemCode;

		send_spawnitemobj_packet(s_socket, SocketTransform.GetLocation(),
			FollowCamera->GetComponentRotation(), SocketTransform.GetScale3D(),
			HotKeyItemCode, SavedHotKeySlotNum);

	}
	else
	{
		FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");
		FRotator CameraRotate = FollowCamera->GetComponentRotation();
		FVector SocketLocation = SocketTransform.GetLocation();

		CameraRotate.Pitch += 14.f;

		FTransform trans(CameraRotate.Quaternion(), SocketLocation);

		int HotKeyItemCode = mInventory->mSlots[SavedHotKeySlotNum].ItemClass.ItemCode;
		FName path = AInventory::ItemCodeToItemBombPath(HotKeyItemCode);

		UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
		if (nullptr != bomb)
		{
			bomb->BombOwner = this;
			bomb->CustomInitialize.Broadcast();
			send_spawnitemobj_packet(s_socket, trans.GetLocation()
				, trans.GetRotation().Rotator(), trans.GetScale3D(),
				HotKeyItemCode, SavedHotKeySlotNum);
			mInventory->RemoveItemAtSlotIndex(SavedHotKeySlotNum, 1);

			bomb->ProjectileMovementComponent->Activate();
		}
		else {

			UE_LOG(LogTemp, Error, TEXT("Bomb can't Spawn! ItemCode : %d"), HotKeyItemCode);
			UE_LOG(LogTemp, Error, TEXT("Bomb can't Spawn! ItemCode String : %s"), *path.ToString());
		}

	}

}


void AMyCharacter::Throw(const FVector& location,FRotator rotation, const int& fruitType, const int& fruitid)
{
	
	FName path = AInventory::ItemCodeToItemBombPath(fruitType);
	if (11 == fruitType)
	{

		FTransform trans(rotation.Quaternion(), location);
		UClass* GenerateBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* banana = GetWorld()->SpawnActor<AProjectile>(GenerateBP, trans);
		if (nullptr != banana)
		{
			Network::GetNetwork()->mBanana[fruitid] = banana;
			banana->uniqueID = fruitid;
			banana->BombOwner = this;
			banana->ProjectileMovementComponent->Activate();
			
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Banana can't Spawn! ItemCode String : %s"), *path.ToString());
		}

	}
	else {
		FTransform trans(rotation.Quaternion(), location);
		UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
		if (nullptr != bomb)
		{
			bomb->BombOwner = this;
			bomb->CustomInitialize.Broadcast();
			bomb->ProjectileMovementComponent->Activate();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Bomb can't Spawn! ItemCode String : %s"), *path.ToString());
		}
	}	
}

void AMyCharacter::ThrowInAIMode(const FVector& location, FRotator rotation, const int& fruitType, const int& fruitid)
{

	FName path = AInventory::ItemCodeToItemBombPathForAI(fruitType);
	if (11 == fruitType)
	{
		FTransform trans(rotation.Quaternion(), location);
		UClass* GenerateBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* banana = GetWorld()->SpawnActor<AProjectile>(GenerateBP, trans);
		if (nullptr != banana)
		{
			Network::GetNetwork()->mBanana[fruitid] = banana;
			banana->uniqueID = fruitid;
			banana->BombOwner = this;
			banana->ProjectileMovementComponent->Activate();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Banana can't Spawn! ItemCode String : %s"), *path.ToString());
		}

	}
	else {
		FTransform trans(rotation.Quaternion(), location);
		UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
		if (nullptr != bomb)
		{
			bomb->BombOwner = this;
			bomb->CustomInitialize.Broadcast();
			bomb->ProjectileMovementComponent->Activate();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Bomb can't Spawn! ItemCode String : %s"), *path.ToString());
		}
	}
}




float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Apply damage�� ����Ǵ� take damage �Լ�.
	// ��� ���ô� �Ʒ��� ����.
	/*
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		if (0 == (PointDamageEvent->HitInfo.BoneName).Compare(FName(TEXT("Head"))))
		{
			Damage *= 5; // ���� ������ Head��, ������ 5��.
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
	
	CurrentHP -= Damage;
	*/

	//������ ������ ��ź�� ��� - ��κ��� ���
	AProjectile* projectile = Cast<AProjectile>(DamageCauser);
	if (projectile != nullptr)
	{
		//UE_LOG(LogTemp, Log, TEXT("Take Damage : Not Me Hit"));
		if (GetController()->IsPlayerController())
		{
			if (nullptr != projectile->BombOwner)
			{
				send_hitmyself_packet(s_socket, projectile->BombOwner->c_id, projectile->_fType);
				//UE_LOG(LogTemp, Log, TEXT("Take Damage : NotifyHit"));
			}
		}
	}

	//������ ������ ����� ��� - �������� ������ �޾��� ���
	ABaseCharacter* DMGCauserCharacter = Cast<ABaseCharacter>(DamageCauser);
	if (nullptr != DMGCauserCharacter)
	{
		//UE_LOG(LogTemp, Log, TEXT("Take Damage : Not Me Hit"));
		if (GetController()->IsPlayerController())
		{
			int m_ftype = static_cast<int>(DamageEvent.DamageTypeClass.GetDefaultObject()->DamageFalloff);
			send_hitmyself_packet(s_socket, DMGCauserCharacter->c_id, m_ftype);
			//UE_LOG(LogTemp, Log, TEXT("Take Damage : NotifyHit %d"), m_ftype);
		}
	}

	return Damage;
}



UMainWidget* AMyCharacter::MakeMainHUD()
{
	if (mMainWidget == nullptr)
	{
		
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MMainWidget.MMainWidget_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();

		FSoftClassPath SlotWidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MInventorySlotWidget.MInventorySlotWidget_C'"));
		auto SlotWidgetClass = SlotWidgetSource.TryLoadClass<UUserWidget>();
		if (nullptr == WidgetClass)
		{
			UE_LOG(LogTemp,Warning,TEXT("MainWidget Source is invalid !! check '/Game/Widget/MMainWidget.MMainWidget_C'"));
			return nullptr;
		}
		if (nullptr == SlotWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("SlotWidget Source is invalid !! check '/Game/Widget/MInventorySlotWidget.MInventorySlotWidget_C'"));
			return nullptr;
		}

		mMainWidget = CreateWidget<UMainWidget>(GetWorld(), WidgetClass);
		if (mMainWidget != nullptr)
		{
			//... Do Something
			mMainWidget->WidgetType = GameState;
			mMainWidget->mInventory = mInventory;
			mInventory->mMainWidget = mMainWidget;
			mMainWidget->mOwnerCharacter = this;
			for (int i = 0; i < 5; ++i)
			{
				auto slot = CreateWidget<UInventorySlotWidget>(GetWorld(), SlotWidgetClass);
				slot->inventoryRef = mInventory;
				slot->mIndex = i;
				slot->Update();
				mMainWidget->InventoryBar->AddChildToHorizontalBox(slot);
				mMainWidget->minventorySlot.Add(slot);
			}
			mMainWidget->AddToViewport();//Nativecontruct ȣ�� ������.
			mMainWidget->SetVisibility(ESlateVisibility::Visible);
			mMainWidget->minventorySlot[0]->Select();
			return mMainWidget;
		}
		else {
			return mMainWidget;
		}
	}
	else {
		return mMainWidget;
	}
}


void AMyCharacter::MakeLoadingHUD()
{
	FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MLoadingWidget.MLoadingWidget_C'"));
	auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
	if (nullptr == WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainWidget Source is invalid !! check '/Game/Widget/MLoadingWidget.MLoadingWidget_C'"));
		return;
	}
	mLoadingWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	mLoadingWidget->AddToViewport();
}

void AMyCharacter::ShowLoginHUD()
{
	if (!IsValid(mLoginWidget))
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MLoginWidget.MLoginWidget_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		if (nullptr == WidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("MainWidget Source is invalid !! check '/Game/Widget/MLoginWidget.MLoginWidget_C'"));
			return;
		}
		mLoginWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	}
	FInputModeUIOnly gamemode;
	auto controller = GetWorld()->GetFirstPlayerController();
	if (nullptr != controller)
	{
		controller->SetInputMode(gamemode);
		controller->SetShowMouseCursor(true);
	}
	mLoginWidget->AddToViewport();
}

void AMyCharacter::ShowMatchHUD()
{
	if (!IsValid(mMatchWidget))
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MGameMatchWidget.MGameMatchWidget_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		if (nullptr == WidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("MainWidget Source is invalid !! check '/Game/Widget/MGameMatchWidget.MGameMatchWidget_C'"));
			return;
		}
		mMatchWidget = CreateWidget<UGameMatchWidget>(GetWorld(), WidgetClass);
	}
	FInputModeUIOnly gamemode;
	auto controller = GetWorld()->GetFirstPlayerController();
	if (nullptr != controller)
	{
		controller->SetInputMode(gamemode);
		controller->SetShowMouseCursor(true);
	}
	mMatchWidget->AddToViewport();
}

void AMyCharacter::EquipSkin()
{
	UStaticMesh* skinmesh = nullptr;
	switch (skinType)
	{
	case 1:
		SkinParts->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("NifeSkinSocket"));
		skinmesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/Assets/CharacterSkins/Knife/Nife_0.Nife_0'")));
		break;
	case 2:
		SkinParts->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("CandySkinSocket"));
		skinmesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Assets/CharacterSkins/Candy/candy.candy")));
		break;
	case 3:
		SkinParts->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("LeafSkinSocket"));
		skinmesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/Assets/CharacterSkins/Leaf/leaf.leaf'")));
		break;
	case 4:
		SkinParts->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("StrawSkinSocket"));
		skinmesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/Assets/CharacterSkins/Straw/straw.straw'")));
		break;
	case 5:
		SkinParts->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("SunflowerSkinSocket"));
		skinmesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("StaticMesh'/Game/Assets/CharacterSkins/SunFlower/SM_SunFlower.SM_SunFlower'")));
		break;
	}
	if (nullptr != skinmesh)
	{
		SkinParts->SetStaticMesh(skinmesh);
		SkinParts->SetHiddenInGame(false);
	}
}

















bool AMyCharacter::ConnServer()
{
	Super::ConnServer();
	s_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if(Network::GetNetwork()->GameServerPort != -1)
		server_addr.sin_port = htons(Network::GetNetwork()->GameServerPort);
	else
		server_addr.sin_port = htons(GAMESERVER_PORT);

	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	int rt = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		//error_display(err_num);
		//system("pause");
		//UE_LOG(LogTemp, Error, TEXT("Conn Error %d"), err_num);
		//exit(0);
		closesocket(s_socket);
		return false;
	}

	recv_expover.setId(static_cast<unsigned char>(overID));

	DWORD recv_flag = 0;
	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), recv_Gamecallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
			return false;
		}
	}
	return true;
}

void AMyCharacter::recvPacket()
{
	Super::recvPacket();
	DWORD recv_flag = 0;
	ZeroMemory(&recv_expover.getWsaOver(), sizeof(recv_expover.getWsaOver()));

	recv_expover.getWsaBuf().buf = reinterpret_cast<char*>(recv_expover.getBuf() + _prev_size);
	recv_expover.getWsaBuf().len = BUFSIZE - _prev_size;

	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), recv_Gamecallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}




bool AMyCharacter::ConnLobbyServer()
{
	Super::ConnLobbyServer();
	l_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&l_server_addr, sizeof(l_server_addr));
	l_server_addr.sin_family = AF_INET;
	l_server_addr.sin_port = htons(LOBBYSERVER_PORT);

	inet_pton(AF_INET, SERVER_ADDR, &l_server_addr.sin_addr);
	int rt = connect(l_socket, reinterpret_cast<sockaddr*>(&l_server_addr), sizeof(l_server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		//error_display(err_num);
		//system("pause");
		//UE_LOG(LogTemp, Error, TEXT("Conn Error %d"), err_num);
		//exit(0);
		closesocket(l_socket);
		return false;
	}

	l_recv_expover.setId(static_cast<unsigned char>(0));

	DWORD recv_flag = 0;
	int ret = WSARecv(l_socket, &l_recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &l_recv_expover.getWsaOver(), recv_Lobbycallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
			return false;
		}
	}
	return true;
}


void AMyCharacter::recvLobbyPacket()
{
	Super::recvLobbyPacket();
	DWORD recv_flag = 0;
	ZeroMemory(&l_recv_expover.getWsaOver(), sizeof(l_recv_expover.getWsaOver()));

	l_recv_expover.getWsaBuf().buf = reinterpret_cast<char*>(l_recv_expover.getBuf() + l_prev_size);
	l_recv_expover.getWsaBuf().len = BUFSIZE - l_prev_size;

	int ret = WSARecv(l_socket, &l_recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &l_recv_expover.getWsaOver(), recv_Lobbycallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}