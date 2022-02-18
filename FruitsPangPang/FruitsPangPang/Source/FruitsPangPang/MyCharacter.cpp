// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Tree.h"
#include "Punnet.h"
#include "Inventory.h"
#include "InventorySlotWidget.h"
#include "MainWidget.h"
#include "Projectile.h"


// Sets default values
AMyCharacter::AMyCharacter()
	:SelectedHotKeySlotNum(0)
	,SavedHotKeyItemCode(0)
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

	GetCharacterMovement()->bOrientRotationToMovement = true; //캐릭터 방향에 따라
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1080.f, 0.f); // 회전
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetController()->IsPlayerController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("other id ")));		
		/*
			Setting Actor Params Before SpawnActor's BeginPlay
		*/
		FName path = TEXT("Blueprint'/Game/Inventory/Inventory_BP.Inventory_BP_C'"); //_C를 꼭 붙여야 된다고 함.
		UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
		FTransform spawnLocAndRot{ GetActorLocation() };
		mInventory = GetWorld()->SpawnActorDeferred<AInventory>(GeneratedInventoryBP, spawnLocAndRot);
		mInventory->mCharacter = this;	// ExposeOnSpawn하고 SpawnActor에서 값 넣어주는게 C++로 짜면 이런식 인듯
		mInventory->mAmountOfSlots = 5;
		mInventory->FinishSpawning(spawnLocAndRot);

		FItemInfo itemClass;
		itemClass.ItemCode = 1;	//토마토 30개 생성
		itemClass.IndexOfHotKeySlot = 0;
		itemClass.Name = mInventory->ItemCodeToItemName(1);
		itemClass.Icon = mInventory->ItemCodeToItemIcon(1);

		mInventory->UpdateInventorySlot(itemClass, 30);

		itemClass.ItemCode = 3;	//수박 30개 생성
		itemClass.IndexOfHotKeySlot = 1;
		itemClass.Name = mInventory->ItemCodeToItemName(3);
		itemClass.Icon = mInventory->ItemCodeToItemIcon(3);
		mInventory->UpdateInventorySlot(itemClass, 30);

		Network::GetNetwork()->mMyCharacter = this;
		if (Network::GetNetwork()->init())
		{
			Network::GetNetwork()->C_Recv();
			Network::GetNetwork()->send_login_packet();
		}
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
			auto pos = GetTransform().GetLocation();
			auto rot = GetTransform().GetRotation();
			Network::GetNetwork()->send_move_packet(pos.X, pos.Y, pos.Z, rot, GroundSpeedd);
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

void AMyCharacter::AnyKeyPressed(FKey Key)
{
	if (Key == EKeys::One)
	{
		UE_LOG(LogTemp, Log, TEXT("One Hitted"));
		int tmp = SelectedHotKeySlotNum;
		SelectedHotKeySlotNum = 0;
		if (tmp != SelectedHotKeySlotNum)
		{
			mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
			mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
			Network::GetNetwork()->send_change_hotkeyslot_packet(SelectedHotKeySlotNum);
		}
	}
	else if (Key == EKeys::Two)
	{
		UE_LOG(LogTemp, Log, TEXT("two Hitted"));
		int tmp = SelectedHotKeySlotNum;
		SelectedHotKeySlotNum = 1;
		if (tmp != SelectedHotKeySlotNum)
		{
			mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
			mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
			Network::GetNetwork()->send_change_hotkeyslot_packet(SelectedHotKeySlotNum);
		}
	}
	else if (Key == EKeys::MouseScrollDown)
	{
		UE_LOG(LogTemp, Log, TEXT("Wheel Down"));
		int tmp = SelectedHotKeySlotNum;
		SelectedHotKeySlotNum = max(SelectedHotKeySlotNum - 1, 0);
		if (tmp != SelectedHotKeySlotNum)
		{
			mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
			mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
			Network::GetNetwork()->send_change_hotkeyslot_packet(SelectedHotKeySlotNum);
		}
	}
	else if (Key == EKeys::MouseScrollUp)
	{
		UE_LOG(LogTemp, Log, TEXT("Wheel Up"));
		int tmp = SelectedHotKeySlotNum;
		SelectedHotKeySlotNum = min(SelectedHotKeySlotNum + 1, 4);
		if (tmp != SelectedHotKeySlotNum)
		{
			mInventory->mMainWidget->minventorySlot[tmp]->UnSelect();
			mInventory->mMainWidget->minventorySlot[SelectedHotKeySlotNum]->Select();
			Network::GetNetwork()->send_change_hotkeyslot_packet(SelectedHotKeySlotNum);
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
			mInventory->RemoveItemAtSlotIndex(SelectedHotKeySlotNum, 1);
			if (c_id == Network::GetNetwork()->mId) {
				Network::GetNetwork()->send_anim_packet(Network::AnimType::Throw);
				Network::GetNetwork()->send_useitem_packet(SelectedHotKeySlotNum, 1);
			}
			bAttacking = true;

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && ThrowMontage)
			{
				AnimInstance->Montage_Play(ThrowMontage, 2.f);
				AnimInstance->Montage_JumpToSection(FName("Default"), ThrowMontage);

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


void AMyCharacter::Jump()
{
	Super::Jump();
}

void AMyCharacter::Throww()
{
	//Blueprint'/Game/Assets/tomato/Bomb.Bomb'

	FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");
	SocketTransform.GetRotation();
	SocketTransform.GetLocation();
	SocketTransform.GetScale3D();
	//FName path = TEXT("Blueprint'/Game/Bomb/Bomb.Bomb_C'"); //_C를 꼭 붙여야 된다고 함.
	//FName path = TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb_Test.Bomb_Test_C'");
	FName path = mInventory->ItemCodeToItemPath(SavedHotKeyItemCode);

	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	auto bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketTransform);
	Network::GetNetwork()->send_spawnobj_packet(SocketTransform.GetLocation(), SocketTransform.GetRotation(), SocketTransform.GetScale3D());

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	//	FString::Printf(TEXT("My pos: ")));


}


void AMyCharacter::GetFruits()
{
	if (OverlapType)
	{
		Network::GetNetwork()->mTree[OverlapInteractId]->CanHarvest = false;
		Network::GetNetwork()->send_getfruits_tree_packet(OverlapInteractId);
	}
	else{
		Network::GetNetwork()->mPunnet[OverlapInteractId]->CanHarvest = false;
		Network::GetNetwork()->send_getfruits_punnet_packet(OverlapInteractId);
	}
}

void AMyCharacter::SendHitPacket()
{

}

void AMyCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	auto other = Cast<AProjectile>(Other);
	
	if (other != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Not Me Hit"));
		if (GetController()->IsPlayerController())
		{
			Network::GetNetwork()->send_hitmyself_packet();
			UE_LOG(LogTemp, Log, TEXT("NotifyHit"));
		}
	}
}