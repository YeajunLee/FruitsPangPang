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
#include "Projectile.h"
#include "RespawnWindowWidget.h"
#include "RespawnWidget.h"
#include "GameMatchWidget.h"
#include "Particles/ParticleSystemComponent.h "
#include "Sound/SoundBase.h"
#include "PointOfInterestComponent.h"
#include "ScoreWidget.h"
#include "Components/Image.h"
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
	
	GetCharacterMovement()->bOrientRotationToMovement = true; //캐릭터 방향에 따라
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1080.f, 0.f); // 회전
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GreenOnionBag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenOnionBag"));
	GreenOnionBag->SetupAttachment(GetMesh());

	CarrotBag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrotBag"));
	CarrotBag->SetupAttachment(GetMesh());
	

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

	POIcomponent = CreateDefaultSubobject<UPointOfInterestComponent>(TEXT("POIComp"));
	
	// 나중에 안쓸 시 지울것
	/*static ConstructorHelpers::FObjectFinder<UTexture2D> myEnemyIconPath(TEXT("/Game/MiniMap/NothingIMG.NothingIMG"));
	if (myEnemyIconPath.Succeeded())
	{
		myEnemy1Icon = myEnemyIconPath.Object;
	}*/
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{

	Super::BeginPlay();

	GreenOnionBag->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GreenOnionBag->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GreenOnionBag"));
	CarrotBag->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarrotBag->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("CarrotBag"));
	
	GreenOnionBag->SetHiddenInGame(true, false);
	CarrotBag->SetHiddenInGame(true, false);

	if (nullptr == GetController())
	{
		UE_LOG(LogTemp, Error, TEXT("Not controller!! PleaseMake SpawnDefaultController and AutoPossessPlayer EAutoReceiveInput::Disabled"));
		return;
	}
	if (GetController()->IsPlayerController())
	{
		Network::GetNetwork()->mMyCharacter = this;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		//	FString::Printf(TEXT("other id ")));		
		/*
			Setting Actor Params Before SpawnActor's BeginPlay
		*/
		FName path = TEXT("Blueprint'/Game/Inventory/Inventory_BP.Inventory_BP_C'"); //_C를 꼭 붙여야 된다고 함.
		UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		FTransform spawnLocAndRot{ GetActorLocation() };
		mInventory = GetWorld()->SpawnActorDeferred<AInventory>(GeneratedInventoryBP, spawnLocAndRot);
		mInventory->mOwnerCharacter = this;	// ExposeOnSpawn하고 SpawnActor에서 값 넣어주는게 C++로 짜면 이런식 인듯
		mInventory->mAmountOfSlots = 5;
		mInventory->FinishSpawning(spawnLocAndRot);

		//지금은 개발 편의성을 위해 여기 넣었지만, 나중에는 서버에서 패킷을 받았을 때 만들어줄 예정임.
		MakeMainHUD();
		//


		FItemInfo itemClass;
		itemClass.ItemCode = 1;	//토마토 30개 생성
		itemClass.IndexOfHotKeySlot = 0;
		itemClass.Name = AInventory::ItemCodeToItemName(1);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(1);

		mInventory->UpdateInventorySlot(itemClass, 30);

		itemClass.ItemCode = 4;	//수박 30개 생성
		itemClass.IndexOfHotKeySlot = 1;
		itemClass.Name = AInventory::ItemCodeToItemName(4);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(4);
		mInventory->UpdateInventorySlot(itemClass, 30);

		itemClass.ItemCode = 7; //대파 1개 생성
		itemClass.IndexOfHotKeySlot = 2;
		itemClass.Name = AInventory::ItemCodeToItemName(7);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(7);
		mInventory->UpdateInventorySlot(itemClass, 1);


		itemClass.ItemCode = 9;	//두리안 30개 생성
		itemClass.IndexOfHotKeySlot = 3;
		itemClass.Name = AInventory::ItemCodeToItemName(9);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(9);
		mInventory->UpdateInventorySlot(itemClass, 30);

		itemClass.ItemCode = 11; //바나나 1개 생성
		itemClass.IndexOfHotKeySlot = 4;
		itemClass.Name = AInventory::ItemCodeToItemName(11);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(11);
		mInventory->UpdateInventorySlot(itemClass, 200);

		
		
		//if (Network::GetNetwork()->init())
		//{
		//	//Network::GetNetwork()->C_Recv();
		//	send_login_packet(s_socket);
		//}
	}
	
	
}

void AMyCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	if (mInventory)
	{
		mInventory->Destroy();
		mInventory = nullptr;
	}

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

		float PitchClamp = FMath::ClampAngle(Rotation.Pitch, -20.f, 30.f);
		FRotator RotationControl(PitchClamp, Rotation.Yaw, Rotation.Roll);

		if (GetController()->IsPlayerController()) {
			SleepEx(0, true);
			auto pos = GetTransform().GetLocation();
			auto rot = GetTransform().GetRotation();

			send_move_packet(s_socket,pos.X, pos.Y, pos.Z, rot, GroundSpeedd);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			//	FString::Printf(TEXT("MY id : My pos:%f,%f,%f , value : "), pos.X, pos.Y, pos.Z));
			float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();
			GroundSpeedd = CharXYVelocity;
			ShowedInMinimap();
		}
		else {
			
			GroundSpeedd = ServerStoreGroundSpeed;
		}
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		//	FString::Printf(TEXT("char before : %f,%f,%f"), this->GetTransform().GetLocation().X, GetTransform().GetLocation().Y, GetTransform().GetLocation().Z));
		auto a = GetTransform().GetLocation().Y;
		Controller->SetControlRotation(RotationControl);
		if (a != GetTransform().GetLocation().Y)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			//	FString::Printf(TEXT("char after : %f,%f,%f"), this->GetTransform().GetLocation().X, GetTransform().GetLocation().Y, GetTransform().GetLocation().Z));

		}

		//Update GroundSpeedd (22-04-05)
		
		
	}


	
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); //플레이어 입력 구성 요소가 유효한지 확인하는 매크로

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
		send_change_hotkeyslot_packet(s_socket, SelectedHotKeySlotNum);
	}
}

void AMyCharacter::AnyKeyPressed(FKey Key)
{
	if (bAttacking) return;
	if (Key == EKeys::One)
	{
		UE_LOG(LogTemp, Log, TEXT("One Hitted"));
		DropSwordAnimation();
		ChangeSelectedHotKey(0);		
	}
	else if (Key == EKeys::Two)
	{
		UE_LOG(LogTemp, Log, TEXT("two Hitted"));
		if (mInventory->IsSlotValid(2) && SelectedHotKeySlotNum == 2)
		{
			DropSwordAnimation();
		}
		DropSwordAnimation();
		ChangeSelectedHotKey(1);
		
	}
	else if (Key == EKeys::Three)
	{
		UE_LOG(LogTemp, Log, TEXT("three Hitted"));
		ChangeSelectedHotKey(2);
		if (mInventory->IsSlotValid(SelectedHotKeySlotNum))
		{
			PickSwordAnimation();
		}

	}
	else if (Key == EKeys::Four)
	{
		UE_LOG(LogTemp, Log, TEXT("Four Hitted"));
		if (mInventory->IsSlotValid(2) && SelectedHotKeySlotNum==2)
		{
			DropSwordAnimation();
		}
		DropSwordAnimation();
		ChangeSelectedHotKey(3);
		
	}
	else if (Key == EKeys::Five)
	{
		UE_LOG(LogTemp, Log, TEXT("Five Hitted"));
		DropSwordAnimation();
		ChangeSelectedHotKey(4);
	}

	else if (Key == EKeys::MouseScrollUp)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		//SavedHotKeyItemCode = mInventory->mSlots[SelectedHotKeySlotNum].ItemClass.ItemCode;

		UE_LOG(LogTemp, Log, TEXT("Wheel Down"));
		int tmp = SelectedHotKeySlotNum;
		DropSwordAnimation();
		SelectedHotKeySlotNum = max(SelectedHotKeySlotNum - 1, 0);
		if (tmp != SelectedHotKeySlotNum)
		{
			mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
			mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
			send_change_hotkeyslot_packet(s_socket, SelectedHotKeySlotNum);
			
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
		UE_LOG(LogTemp, Log, TEXT("Wheel Up"));
		int tmp = SelectedHotKeySlotNum;
		DropSwordAnimation();
		SelectedHotKeySlotNum = min(SelectedHotKeySlotNum + 1, 4);
		if (tmp != SelectedHotKeySlotNum)
		{
			mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
			mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
			send_change_hotkeyslot_packet(s_socket, SelectedHotKeySlotNum);
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

		//auto pos = GetTransform().GetLocation();
		//auto rot = GetTransform().GetRotation();
		//
		//send_move_packet(pos.X,pos.Y,pos.Z,rot, GroundSpeed,MOVE_FORWARD);

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		//	FString::Printf(TEXT("after x y z : %f %f %f "), pos.X, pos.Y, pos.Z));
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
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		//	FString::Printf(TEXT("My pos:%f,%f,%f , value : %f "), Direction.X, Direction.Y, Direction.Z, value));

		//SetActorLocation(GetTransform().GetLocation() * 0.1);
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
			//UE_LOG(LogTemp, Log, TEXT("Tree Fruit"));
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
			//UE_LOG(LogTemp, Log, TEXT("Punnet Fruit"));
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
	}
}

void AMyCharacter::LMBDown()
{
	bLMBDown = true;

	//UE_LOG(LogTemp, Log, TEXT("lmbdown"));

	Attack();
}

void AMyCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMyCharacter::Jump()
{
	Super::Jump();
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

//죽는 애니메이션(22-04-05)
//스테이트 머신으로 돌림(22-04-30)
//void AMyCharacter::Die()
//{
//	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//
//	if (AnimInstance && DeathMontage)
//	{
//		AnimInstance->Montage_Play(DeathMontage, 1.f);
//		AnimInstance->Montage_JumpToSection(FName("Default"), DeathMontage);
//		UE_LOG(LogTemp, Warning, TEXT("die!!"));
//	}
//}


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
				bStepBanana = true;
				banana->Destroy();
				DisableInput(Cast<APlayerController>(this));
				if (P_Star && P_Star->Template)
				{
					P_Star->ToggleActive();
				}
				UGameplayStatics::PlaySoundAtLocation(this, dizzySound, GetActorLocation());
				GetWorld()->GetTimerManager().SetTimer(TimerHandle,this ,&AMyCharacter::onTimerEnd , 2.5, false);
				
			}
		}
	}
}

void AMyCharacter::GreenOnionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::GreenOnionBeginOverlap(OverlappedComp,OtherActor,OtherComp,OtherBodyIndex, bFromSweep,SweepResult);
	UE_LOG(LogTemp, Log, TEXT("%s"), *UKismetSystemLibrary::GetDisplayName(OtherComp));
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
				//	//원래는 피해감소 옵션이지만, 사용하지 않으니 내 입맛대로 fruitType을 보내주도록 한다.
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
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("end"));
	//}
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
		UE_LOG(LogTemp, Warning, TEXT("GO_AttackEnd"));

		if (!this->SM_GreenOnion->bHiddenInGame)
		{
			//원래는 피해감소 옵션이지만, 사용하지 않으니 내 입맛대로 fruitType을 보내주도록 한다.
			dmgCauser.GetDefaultObject()->DamageFalloff = 7.0f;
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Daepa is not Equipped %d"), c_id);

		UGameplayStatics::ApplyDamage(victim, 1, GetInstigatorController(), this, dmgCauser);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("start :"));
		UE_LOG(LogTemp, Log, TEXT("Damage Type %d"), dmgCauser.GetDefaultObject()->DamageFalloff);
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
		UE_LOG(LogTemp, Warning, TEXT("CarrotAttackEnd"));
		if (!this->SM_Carrot->bHiddenInGame)
		{
			dmgCauser.GetDefaultObject()->DamageFalloff = 8.0f;
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Dangeun is not Equipped %d"), c_id);
		UGameplayStatics::ApplyDamage(victim, 1, GetInstigatorController(), this, dmgCauser);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("start :"));
		UE_LOG(LogTemp, Log, TEXT("Damage Type %d"), dmgCauser.GetDefaultObject()->DamageFalloff);
	}
	DamagedActorCollector.clear();
	SM_Carrot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AMyCharacter::ShowedInMinimap()
{
	
	if (mInventory->mMainWidget->mScoreWidget != nullptr)
	{
		if (mInventory->mMainWidget->mScoreWidget->ScoreBoard[0].GetCharacter()->c_id == this->c_id)
		{
			POIcomponent->isOn = false;
		}
		else
			POIcomponent->isOn = true;
	}
}

void AMyCharacter::Throw()
{
	if (SavedHotKeySlotNum == 4)
	{
		FTransform SocketTransform = GetMesh()->GetSocketTransform("BananaSocket");
		FRotator CameraRotate = FollowCamera->GetComponentRotation();
		CameraRotate.Pitch += 14;
		FTransform trans(CameraRotate.Quaternion(), SocketTransform.GetLocation());
		int HotKeyItemCode = mInventory->mSlots[SavedHotKeySlotNum].ItemClass.ItemCode;
		FName path = AInventory::ItemCodeToItemBombPath(11);

		UClass* GenerateBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* banana = GetWorld()->SpawnActor<AProjectile>(GenerateBP, trans);
		if (nullptr != banana)
		{
			banana->uniqueID = Network::GetNetwork()->getNewBananaId();
			send_spawnitemobj_packet(s_socket, SocketTransform.GetLocation(),
				FollowCamera->GetComponentRotation(), SocketTransform.GetScale3D(),
				HotKeyItemCode, SavedHotKeySlotNum, banana->uniqueID);
			mInventory->RemoveItemAtSlotIndex(SavedHotKeySlotNum, 1);
			banana->BombOwner = this;
			banana->ProjectileMovementComponent->Activate();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Banana can't Spawn! ItemCode String : %s"), *path.ToString());
		}

	}
	else
	{
		FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");
		
		FRotator CameraRotate = FollowCamera->GetComponentRotation();
		CameraRotate.Pitch += 14;
		FTransform trans(CameraRotate.Quaternion(), SocketTransform.GetLocation());
		int HotKeyItemCode = mInventory->mSlots[SavedHotKeySlotNum].ItemClass.ItemCode;
		FName path = AInventory::ItemCodeToItemBombPath(HotKeyItemCode);
		send_spawnitemobj_packet(s_socket, SocketTransform.GetLocation()
			, FollowCamera->GetComponentRotation(), SocketTransform.GetScale3D(), 
			HotKeyItemCode, SavedHotKeySlotNum,0);

		UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
		if (nullptr != bomb)
		{
			mInventory->RemoveItemAtSlotIndex(SavedHotKeySlotNum, 1);

			bomb->BombOwner = this;
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
		FTransform SocketTransform = GetMesh()->GetSocketTransform("BananaSocket");
		FRotator CameraRotate = FollowCamera->GetComponentRotation();
		CameraRotate.Pitch += 14;
		FTransform trans(CameraRotate.Quaternion(), SocketTransform.GetLocation());
		//FName path = AInventory::ItemCodeToItemBombPath(11);

		UClass* GenerateBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* banana = GetWorld()->SpawnActor<AProjectile>(GenerateBP, trans);
		if (nullptr != banana)
		{
			banana->uniqueID = fruitid;
			banana->BombOwner = this;
			banana->ProjectileMovementComponent->Activate();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Banana can't Spawn! ItemCode String : %s"), *path.ToString());
		}

	}
	else {
		rotation.Pitch += 14;
		FTransform trans(rotation.Quaternion(), location);
		UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
		if (nullptr != bomb)
		{
			bomb->BombOwner = this;
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
		FTransform SocketTransform = GetMesh()->GetSocketTransform("BananaSocket");
		FRotator CameraRotate = FollowCamera->GetComponentRotation();
		CameraRotate.Pitch += 14;
		FTransform trans(CameraRotate.Quaternion(), SocketTransform.GetLocation());
		//FName path = AInventory::ItemCodeToItemBombPath(11);

		UClass* GenerateBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* banana = GetWorld()->SpawnActor<AProjectile>(GenerateBP, trans);
		if (nullptr != banana)
		{
			banana->uniqueID = fruitid;
			banana->BombOwner = this;
			banana->ProjectileMovementComponent->Activate();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Banana can't Spawn! ItemCode String : %s"), *path.ToString());
		}

	}
	else {
		rotation.Pitch += 14;
		FTransform trans(rotation.Quaternion(), location);
		UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
		if (nullptr != bomb)
		{
			bomb->BombOwner = this;
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

	// Apply damage와 연계되는 take damage 함수.
	// 사용 예시는 아래와 같다.
	/*
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		if (0 == (PointDamageEvent->HitInfo.BoneName).Compare(FName(TEXT("Head"))))
		{
			Damage *= 5; // 맞은 부위가 Head면, 데미지 5배.
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
	
	CurrentHP -= Damage;
	*/

	//데미지 입힌게 폭탄일 경우 - 대부분의 경우
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

	//데미지 입힌게 사람인 경우 - 근접무기 공격을 받았을 경우
	ABaseCharacter* DMGCauserCharacter = Cast<ABaseCharacter>(DamageCauser);
	if (nullptr != DMGCauserCharacter)
	{
		//UE_LOG(LogTemp, Log, TEXT("Take Damage : Not Me Hit"));
		if (GetController()->IsPlayerController())
		{
			int m_ftype = static_cast<int>(DamageEvent.DamageTypeClass.GetDefaultObject()->DamageFalloff);
			send_hitmyself_packet(s_socket, DMGCauserCharacter->c_id, m_ftype);
			UE_LOG(LogTemp, Log, TEXT("Take Damage : NotifyHit %d"), m_ftype);
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
			mMainWidget->AddToViewport();//Nativecontruct 호출 시점임.
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
		UE_LOG(LogTemp, Error, TEXT("Conn Error %d"), err_num);
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
		UE_LOG(LogTemp, Error, TEXT("Conn Error %d"), err_num);
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