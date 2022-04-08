// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
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
#include "Kismet/GameplayStatics.h"



// Sets default values
AMyCharacter::AMyCharacter()
	:s_connected(false)
	, bInteractDown(false)
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

	GreenOnionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenOnionMesh"));
	GreenOnionMesh->SetupAttachment(GetRootComponent());
	ConstructorHelpers::FObjectFinder<UStaticMesh> GreenOnionAsset(TEXT("/Game/Assets/Fruits/BigGreenOnion/SM_GreenOnion.SM_GreenOnion"));
	if (GreenOnionAsset.Succeeded())
		GreenOnionMesh->SetStaticMesh(GreenOnionAsset.Object);

	CarrotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrotMesh"));
	CarrotMesh->SetupAttachment(GetRootComponent());
	ConstructorHelpers::FObjectFinder<UStaticMesh> CarrotAsset(TEXT("/Game/Assets/Fruits/Carrot/SM_Carrot.SM_Carrot"));
	if (CarrotAsset.Succeeded())
		CarrotMesh->SetStaticMesh(CarrotAsset.Object);

	GreenOnionMesh->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlapBegin);
	GreenOnionMesh->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::OnOverlapEnd);
	
	CarrotMesh->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlapBegin);
	CarrotMesh->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::OnOverlapEnd);

	GreenOnionBag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenOnionBag"));
	GreenOnionBag->SetupAttachment(GetMesh());

	CarrotBag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrotBag"));
	CarrotBag->SetupAttachment(GetMesh());
	
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{

	Super::BeginPlay();
	
	GreenOnionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarrotMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GreenOnionMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GreenOnionSocket"));
	//GreenOnionMesh->AttachTo(GetMesh(), TEXT("GreenOnionSocket"), EAttachLocation::SnapToTargetIncludingScale, false);
	//CarrotMesh->AttachTo(GetMesh(), TEXT("CarrotSocket"), EAttachLocation::SnapToTargetIncludingScale, false);
	CarrotMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("CarrotSocket"));
	
	GreenOnionMesh->SetHiddenInGame(true, false);
	CarrotMesh->SetHiddenInGame(true, false);
	
	GreenOnionMesh->SetGenerateOverlapEvents(true);
	CarrotMesh->SetGenerateOverlapEvents(true);
	GreenOnionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CarrotMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	GreenOnionBag->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GreenOnionBag->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GreenOnionBag"));
	CarrotBag->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarrotBag->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("CarrotBag"));
	
	GreenOnionBag->SetHiddenInGame(true, false);
	CarrotBag->SetHiddenInGame(true, false);

	
	
	if (GetController()->IsPlayerController())
	{
		Network::GetNetwork()->mMyCharacter = this;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("other id ")));		
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

		//mInventory->mMainWidget->MinimapBox->AddChildToHorizontalBox(CreateWidget<UUserWidget>(GetWorld(), p));
		//mInventory->mMainWidget->MinimapBox->SetVisibility(ESlateVisibility::Hidden);


		FItemInfo itemClass;
		itemClass.ItemCode = 2;	//토마토 30개 생성
		itemClass.IndexOfHotKeySlot = 0;
		itemClass.Name = AInventory::ItemCodeToItemName(2);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(2);

		mInventory->UpdateInventorySlot(itemClass, 30);

		itemClass.ItemCode = 6;	//수박 30개 생성
		itemClass.IndexOfHotKeySlot = 1;
		itemClass.Name = AInventory::ItemCodeToItemName(6);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(6);
		mInventory->UpdateInventorySlot(itemClass, 30);

		itemClass.ItemCode = 7; //대파 1개 생성
		itemClass.IndexOfHotKeySlot = 2;
		itemClass.Name = AInventory::ItemCodeToItemName(7);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(7);
		mInventory->UpdateInventorySlot(itemClass, 1);


		itemClass.ItemCode = 10;	//두리안 30개 생성
		itemClass.IndexOfHotKeySlot = 3;
		itemClass.Name = AInventory::ItemCodeToItemName(10);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(10);
		mInventory->UpdateInventorySlot(itemClass, 30);

		itemClass.ItemCode = 11; //바나나 1개 생성
		itemClass.IndexOfHotKeySlot = 4;
		itemClass.Name = AInventory::ItemCodeToItemName(11);
		itemClass.Icon = AInventory::ItemCodeToItemIcon(11);
		mInventory->UpdateInventorySlot(itemClass, 1);

		

		//if (Network::GetNetwork()->init())
		//{
		//	//Network::GetNetwork()->C_Recv();
		//	Network::GetNetwork()->send_login_packet(s_socket);
		//}
	}
	else {
		Network::GetNetwork()->mOtherCharacter[Network::GetNetwork()->WorldCharacterCnt] = this;
		Network::GetNetwork()->WorldCharacterCnt++;
	}


}

void AMyCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	if (mInventory)
	{
		mInventory->Destroy();
		mInventory = nullptr;
	}

	closesocket(s_socket);
	Network::GetNetwork()->release();
	//Network::GetNetwork().reset();
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
			auto pos = GetTransform().GetLocation();
			auto rot = GetTransform().GetRotation();
			Network::GetNetwork()->send_move_packet(s_socket,pos.X, pos.Y, pos.Z, rot, GroundSpeedd);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			//	FString::Printf(TEXT("MY id : My pos:%f,%f,%f , value : "), pos.X, pos.Y, pos.Z));
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
		float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();
		GroundSpeedd = CharXYVelocity;
		
	}

	//SwordInTheBag();
	

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
		Network::GetNetwork()->send_change_hotkeyslot_packet(s_socket, SelectedHotKeySlotNum);
	}
}

void AMyCharacter::AnyKeyPressed(FKey Key)
{
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
			PickSwordAnimation();
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
			PickSwordAnimation();
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
	else if (Key == EKeys::MouseScrollDown)
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
			Network::GetNetwork()->send_change_hotkeyslot_packet(s_socket, SelectedHotKeySlotNum);
			
			if (SelectedHotKeySlotNum == 2 && mInventory->IsSlotValid(SelectedHotKeySlotNum))
			{
				PickSwordAnimation();
			}
			if (SelectedHotKeySlotNum == 1 && mInventory->IsSlotValid(2))
			{
				PickSwordAnimation();
			}
		}
	}
	else if (Key == EKeys::MouseScrollUp)
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
			Network::GetNetwork()->send_change_hotkeyslot_packet(s_socket, SelectedHotKeySlotNum);
			if (SelectedHotKeySlotNum == 2 && mInventory->IsSlotValid(2))
			{
				PickSwordAnimation();
			}
			if (SelectedHotKeySlotNum == 3 && mInventory->IsSlotValid(2))
			{
				PickSwordAnimation();
			}
			
		}
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
		//Network::GetNetwork()->send_move_packet(pos.X,pos.Y,pos.Z,rot, GroundSpeed,MOVE_FORWARD);

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
		//if (Network::GetNetwork()->mTree[OverlapTreeId]->CanHarvest)
		{
			GetFruits();
		}
		bInteractDown = true;
	}
}

void AMyCharacter::InteractUp()
{
	bInteractDown = false;
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
		//임의, mSlots[0]의 0은 나중에 SelectedHotKey 같은 변수명으로 바뀔 예정.
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("Amount: %d"), mInventory->mSlots[SelectedHotKeySlotNum].Amount));
		if (mInventory->mSlots[SelectedHotKeySlotNum].Amount > 0)
		{
			SavedHotKeyItemCode = mInventory->mSlots[SelectedHotKeySlotNum].ItemClass.ItemCode;
			bAttacking = true;

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			
			if (SavedHotKeyItemCode == 7)
			{	
				if (AnimInstance && SlashMontage)
				{
					AnimInstance->Montage_Play(SlashMontage, 1.5f);
					AnimInstance->Montage_JumpToSection(FName("Default"), SlashMontage);
					Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Slash);
					
				}
			}
			else if (SavedHotKeyItemCode == 8)
			{
				if (AnimInstance && StabbingMontage)
				{
					AnimInstance->Montage_Play(StabbingMontage, 1.2f);
					AnimInstance->Montage_JumpToSection(FName("Default"), StabbingMontage);
					Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Stab);
					
				}
			}
			else if (AnimInstance && ThrowMontage)
			{
				mInventory->RemoveItemAtSlotIndex(SelectedHotKeySlotNum, 1);
				AnimInstance->Montage_Play(ThrowMontage, 2.f);
				AnimInstance->Montage_JumpToSection(FName("Default"), ThrowMontage);
				Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Throw);
				Network::GetNetwork()->send_useitem_packet(s_socket, SelectedHotKeySlotNum, 1);
			}
			
		}
		
	}
}

void AMyCharacter::AttackEnd()
{
	bAttacking = false;
	if (bLMBDown)
	{
		Attack();
	}
}

//죽는 애니메이션(22-04-05)
void AMyCharacter::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Default"), DeathMontage);
		UE_LOG(LogTemp, Warning, TEXT("die!!"));
	}
}

void AMyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (GEngine)
		{
			auto p = Cast<AMyCharacter>(OtherActor);
			if (nullptr != p)
			{
				TSubclassOf<UDamageType> dmgCauser;
				dmgCauser = UDamageType::StaticClass();
				
				if (p->GreenOnionMesh->bHiddenInGame)
				{
					//원래는 피해감소 옵션이지만, 사용하지 않으니 내 입맛대로 fruitType을 보내주도록 한다.
					dmgCauser.GetDefaultObject()->DamageFalloff = 7.0f;
				}
				else if (p->CarrotMesh->bHiddenInGame)
				{
					dmgCauser.GetDefaultObject()->DamageFalloff = 8.0f;
				}				
				UGameplayStatics::ApplyDamage(OtherActor, 1, GetInstigatorController(), this, dmgCauser);
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("start :"));
				UE_LOG(LogTemp, Log, TEXT("Damage Type %d"), dmgCauser.GetDefaultObject()->DamageFalloff);
			}
		}
	}
}

void AMyCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("end"));
	}
}


void AMyCharacter::GreenOnionAttackStart()
{
	GreenOnionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMyCharacter::GreenOnionAttackEnd()
{
	GreenOnionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMyCharacter::CarrotAttackStart()
{
	CarrotMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMyCharacter::CarrotAttackEnd()
{
	CarrotMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
			GreenOnionMesh->SetHiddenInGame(false, false);
			GreenOnionBag->SetHiddenInGame(true, false);
			Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::PickSword_GreenOnion);
			break;
		case 8:
			CarrotMesh->SetHiddenInGame(false, false);
			CarrotBag->SetHiddenInGame(true, false);
			Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::PickSword_Carrot);
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

	FItemInfo info;
	bool isempty;
	int amount;
	mInventory->GetItemInfoAtSlotIndex(SelectedHotKeySlotNum, isempty, info, amount);
	switch (info.ItemCode)
	{
	case 7:
		GreenOnionMesh->SetHiddenInGame(true, false);
		GreenOnionBag->SetHiddenInGame(false, false);
		
		break;
	case 8:
		CarrotMesh->SetHiddenInGame(true, false);
		CarrotBag->SetHiddenInGame(false, false);
		
		break;
	}
	Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::DropSword);
}

void AMyCharacter::Throww()
{

	FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");
	FRotator CameraRotate = FollowCamera->GetComponentRotation();
	CameraRotate.Pitch += 14;
	FTransform trans(CameraRotate.Quaternion(), SocketTransform.GetLocation());
	FName path = AInventory::ItemCodeToItemBombPath(SavedHotKeyItemCode);
	Network::GetNetwork()->send_spawnobj_packet(s_socket, SocketTransform.GetLocation(), FollowCamera->GetComponentRotation(), SocketTransform.GetScale3D(), SavedHotKeyItemCode);
	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	auto bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
	bomb->BombOwner = this;
	//FAttachmentTransformRules attachrules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
	//bomb->AttachToComponent(this->GetMesh(), attachrules, "BombSocket");
	//FDetachmentTransformRules Detachrules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative,true);
	//bomb->DetachFromActor(Detachrules);
	bomb->ProjectileMovementComponent->Activate();


	//
	////GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	////	FString::Printf(TEXT("My pos: ")));


}

void AMyCharacter::Throw(const FVector& location, FRotator rotation, const FName& path)
{
	rotation.Pitch += 18;
	FTransform trans(rotation.Quaternion(), location);


	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	auto bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
	bomb->BombOwner = this;
	//FAttachmentTransformRules attachrules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
	//bomb->AttachToComponent(this->GetMesh(), attachrules, "BombSocket");
	//FDetachmentTransformRules Detachrules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative,true);
	//bomb->DetachFromActor(Detachrules);
	bomb->ProjectileMovementComponent->Activate();
}

void AMyCharacter::GetFruits()
{
	Super::GetFruits();
	if (OverlapType)
	{
		Network::GetNetwork()->mTree[OverlapInteractId]->CanHarvest = false;
		Network::GetNetwork()->send_getfruits_tree_packet(s_socket, OverlapInteractId);
		UE_LOG(LogTemp, Log, TEXT("Tree Fruit"));
	}
	else{
		Network::GetNetwork()->mPunnet[OverlapInteractId]->CanHarvest = false;
		Network::GetNetwork()->send_getfruits_punnet_packet(s_socket, OverlapInteractId);
		UE_LOG(LogTemp, Log, TEXT("Punnet Fruit"));
	}
}

void AMyCharacter::SendHitPacket()
{

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

	auto projectile = Cast<AProjectile>(DamageCauser);

	auto DMGCauserCharacter = Cast<ABaseCharacter>(DamageCauser);
	if (projectile != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Take Damage : Not Me Hit"));
		if (GetController()->IsPlayerController())
		{
			if (nullptr != projectile->BombOwner)
			{
				Network::GetNetwork()->send_hitmyself_packet(s_socket, projectile->BombOwner->c_id, projectile->_fType);
				UE_LOG(LogTemp, Log, TEXT("Take Damage : NotifyHit"));
			}
		}
	}

	if (nullptr != DMGCauserCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("Take Damage : Not Me Hit"));
		if (GetController()->IsPlayerController())
		{
			int m_ftype = static_cast<int>(DamageEvent.DamageTypeClass.GetDefaultObject()->DamageFalloff);
			Network::GetNetwork()->send_hitmyself_packet(s_socket, DMGCauserCharacter->c_id, m_ftype);
			UE_LOG(LogTemp, Log, TEXT("Take Damage : NotifyHit %d"), m_ftype);
		}
	}

	return Damage;
}


bool AMyCharacter::ConnServer()
{
	Super::ConnServer();
	s_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

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
	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), recv_callback);
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

	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), recv_callback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}