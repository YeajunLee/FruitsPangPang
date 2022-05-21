// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "AIController_Custom.h"
#include "Inventory.h"
#include "Projectile.h"
#include "Tree.h"
#include "Punnet.h"
#include "Network.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h "
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

TreeInfo::TreeInfo() :
	mTree(nullptr)
	, bIgnored(false)
{

}

TreeInfo::TreeInfo(ATree* tree) :
	mTree(tree)
	, bIgnored(false)
{

}

PunnetInfo::PunnetInfo()
	:mPunnet(nullptr)
	, bIgnored(false)
{

}

PunnetInfo::PunnetInfo(APunnet* punnet)
	:mPunnet(punnet)
	, bIgnored(false)
{

}

// Sets default values
AAICharacter::AAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController_Custom::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //������ ��ġ�ϰų� ���� �����Ǵ� AI�� AIConstrollerCustom�� ���踦 �ްԵȴ�.

	//BananaCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BananaCollisionBox"));
	//BananaCollisionBox->SetupAttachment(GetMesh());
	//BananaCollisionBox->SetRelativeLocation(FVector(0.f, 0.f, -130.f));
	//BananaCollisionBox->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.4f));
	//BananaCollisionBox->SetMobility(EComponentMobility::Movable);
	//BananaCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAICharacter::OnBananaBoxOverlapBegin);
	//BananaBox->OnComponentBeginOverlap.AddDynamic(this, &AAICharacter::OnBoxOverlapBegin);

	BananaCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BananaCollision"));;
	BananaCollision->SetupAttachment(GetMesh());
	BananaCollision->SetRelativeLocation(FVector(0.f, 0.f, -130.f));
	BananaCollision->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.4f));
	BananaCollision->SetMobility(EComponentMobility::Movable);
	BananaCollision->OnComponentBeginOverlap.AddDynamic(this, &AAICharacter::OnBananaBoxOverlapBegin);

	P_Star1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("StarParticle1"));
	P_Star1->SetupAttachment(RootComponent);
	P_Star1->bAutoActivate = false;
	P_Star1->SetRelativeLocation(FVector(50.f, 10.f, 10.f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset1(TEXT("/Game/Assets/Fruits/Banana/P_Stars.P_Stars"));
	if (ParticleAsset1.Succeeded())
	{
		P_Star1->SetTemplate(ParticleAsset1.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> dizzySoundAsset1(TEXT("/Game/Assets/Fruits/Banana/S_dizzy.S_dizzy"));
	if (dizzySoundAsset1.Succeeded())
	{
		dizzySound1 = dizzySoundAsset1.Object;
	}

	//movement = this->GetCharacterMovement();
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();


	overID = Network::GetNetwork()->getNewId();
	UE_LOG(LogTemp, Log, TEXT("Ai Number :%d Genereate"), overID);
	Network::GetNetwork()->mAiCharacter[overID] = this;
	Network::GetNetwork()->init();
	ConnServer();
	Network::GetNetwork()->send_login_packet(s_socket, 1);

	FName path = TEXT("Blueprint'/Game/Inventory/Inventory_BP.Inventory_BP_C'"); //_C�� �� �ٿ��� �ȴٰ� ��.
	UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform spawnLocAndRot{ GetActorLocation() };
	mInventory = GetWorld()->SpawnActorDeferred<AInventory>(GeneratedInventoryBP, spawnLocAndRot);
	mInventory->mOwnerCharacter = this;	// ExposeOnSpawn�ϰ� SpawnActor���� �� �־��ִ°� C++�� ¥�� �̷��� �ε�
	mInventory->mAmountOfSlots = 5;
	mInventory->FinishSpawning(spawnLocAndRot);

	/*
	* �丶�� 5�� ��� ����
	*/
	//FItemInfo itemClass;
	//itemClass.ItemCode = 1;
	//itemClass.IndexOfHotKeySlot = 0;
	//itemClass.Name = AInventory::ItemCodeToItemName(1);
	//itemClass.Icon = AInventory::ItemCodeToItemIcon(1);
	//mInventory->UpdateInventorySlot(itemClass, 5);
	SelectedHotKeySlotNum = 0;

	//ConnServer();

	bIsUndertheTree = false;
}

void AAICharacter::EndPlay(EEndPlayReason::Type Reason)
{
	closesocket(s_socket);
	Network::GetNetwork()->release();
	//Network::GetNetwork().reset();
	UE_LOG(LogTemp, Log, TEXT("ENd Played"));
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto pos = GetTransform().GetLocation();
	auto rot = GetTransform().GetRotation();
	if(overID == 0)
		SleepEx(0, true);
	Network::GetNetwork()->send_move_packet(s_socket, pos.X, pos.Y, pos.Z, rot, GroundSpeed_AI);

	//Update GroundSpeedd (22-04-05)
	float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();
	GroundSpeed_AI = CharXYVelocity;
}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AAICharacter::Attack()
{
	if (!bAttacking)
	{
		AnimInstance = GetMesh()->GetAnimInstance();
		
		//Play Throw Montage	
		if (mInventory->mSlots[SelectedHotKeySlotNum].Amount > 0)
		{
			bAttacking = true;
			SavedHotKeySlotNum = SelectedHotKeySlotNum;
			int HotKeyItemCode = mInventory->mSlots[SelectedHotKeySlotNum].ItemClass.ItemCode;
			UE_LOG(LogTemp, Log, TEXT("%d"), HotKeyItemCode);
			//mInventory->RemoveItemAtSlotIndex(SelectedHotKeySlotNum, 1);
			//if (c_id == Network::GetNetwork()->mId) 		
			//Network::GetNetwork()->send_useitem_packet(s_socket, SelectedHotKeySlotNum, 1);

			if (HotKeyItemCode == 7)
			{
				if (AnimInstance && SlashMontage_AI)
				{
					AnimInstance->Montage_Play(SlashMontage_AI, 1.5f);
					AnimInstance->Montage_JumpToSection(FName("Default"), SlashMontage_AI);
					Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Slash);
				}
			}
			else if (HotKeyItemCode == 8)
			{
				if (AnimInstance && StabMontage_AI)
				{
					AnimInstance->Montage_Play(StabMontage_AI, 1.2f);
					AnimInstance->Montage_JumpToSection(FName("Default"), StabMontage_AI);
					Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Stab);
				}
			}
			
			else if (AnimInstance && ThrowMontage_AI)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Attack!"));
				//UE_LOG(LogTemp, Warning, TEXT("left tomato: %d"), mInventory->mSlots[SelectedHotKeySlotNum].Amount)

				AnimInstance->Montage_Play(ThrowMontage_AI, 2.5f);
				AnimInstance->Montage_JumpToSection(FName("Default"), ThrowMontage_AI);
				Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Throw);
			}

		}

		//������ ��� ���� AddDynamic�� AddUniqueDynamic���� �ٲ�.
		AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AAICharacter::OnAttackMontageEnded);
	}

	
}

//void AAICharacter::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//
//}

//void AAICharacter::AttackEnd()
//{
//	bAttacking = false;
//	Attack();
//}

void AAICharacter::Throw()
{
	FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");

	auto ToTarget = UAIBlueprintHelperLibrary::GetBlackboard(this)->GetValueAsRotator(AAIController_Custom::TrackingTargetKey);

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *ToTarget.ToString());

	//Ai�� �ٶ󺸰� �ִ� ���⺤�ͷ� Rotater�� ������� �װ� ���ϰ� ���ļ� transform�� �����.
	//FRotator aiRotate = GetActorForwardVector().Rotation();

	FTransform trans(ToTarget.Quaternion(), SocketTransform.GetLocation());
	//Network::GetNetwork()->send_spawnitemobj_packet(s_socket, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), SocketTransform.GetScale3D(), HotKeyItemCode,SavedHotKeySlotNum);

	int HotKeyItemCode = mInventory->mSlots[SavedHotKeySlotNum].ItemClass.ItemCode;

	//Ȥ�� ���࿡ �ٳ����� �����°� ������ �����̶�� spawnitemobj_packet�� �� ������ ���ڸ� ���ǹ��� ���� �־���ϹǷ�
	//Mycharacter�� �ٳ��� �����°��� �������ּ��� - ����
	Network::GetNetwork()->send_spawnitemobj_packet(s_socket, SocketTransform.GetLocation(),
		ToTarget, SocketTransform.GetScale3D(), HotKeyItemCode,
		SavedHotKeySlotNum, 0);

	FName path = AInventory::ItemCodeToItemBombPathForAI(HotKeyItemCode);

	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
	if (nullptr != bomb)
	{
		mInventory->RemoveItemAtSlotIndex(SavedHotKeySlotNum, 1);
		bomb->BombOwner = this;
		bomb->ProjectileMovementComponent->Activate();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Bomb can't Spawn! - AI ItemCode : %d"), HotKeyItemCode);
		UE_LOG(LogTemp, Error, TEXT("Bomb can't Spawn! ItemCode String : %s"), *path.ToString());
	}
	
}

void AAICharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackEnd.Broadcast();
}

void AAICharacter::GetFruits()
{
	Super::GetFruits();
	if (OverlapType)
	{
		if (OverlapInteractId != -1)
		{
			Network::GetNetwork()->mTree[OverlapInteractId]->CanHarvest = false;
			Network::GetNetwork()->send_getfruits_tree_packet(s_socket, OverlapInteractId);
			//UE_LOG(LogTemp, Log, TEXT("Tree Fruit"));
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Overlap is -1 But Try GetFruits - Type:Tree"));
		}
	}
	else {
		if (OverlapInteractId != -1)
		{
			Network::GetNetwork()->mPunnet[OverlapInteractId]->CanHarvest = false;
			Network::GetNetwork()->send_getfruits_punnet_packet(s_socket,OverlapInteractId);
			//UE_LOG(LogTemp, Log, TEXT("Punnet Fruit"));
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Overlap is -1 But Try GetFruits - Type:Punnet"));
		}
	}
}


void AAICharacter::OnTimeEnd()
{
	bStepBanana = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	//movement->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AAICharacter::OnBananaBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor)
	{
		AProjectile* banana = Cast<AProjectile>(OtherActor);
		if (nullptr != banana)
		{
			if (banana->_fType == 11)
			{
				bStepBanana = true;
				banana->Destroy();
				GetCharacterMovement()->DisableMovement();
				if (P_Star1 && P_Star1->Template)
				{
					P_Star1->ToggleActive();
				}
				UGameplayStatics::PlaySoundAtLocation(this, dizzySound1, GetActorLocation());
				
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAICharacter::OnTimeEnd, 2.5, false);

			}
		}
	}
}


void AAICharacter::GreenOnionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AAICharacter::GreenOnionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::GreenOnionEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}

void AAICharacter::CarrotBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AAICharacter::CarrotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

float AAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
		//if (GetController()->IsPlayerController())
		{
			if (nullptr != projectile->BombOwner)
			{
				Network::GetNetwork()->send_hitmyself_packet(s_socket, projectile->BombOwner->c_id, projectile->_fType);
				//UE_LOG(LogTemp, Log, TEXT("Take Damage : NotifyHit"));
			}
		}
	}

	//������ ������ ����� ��� - �������� ������ �޾��� ���
	ABaseCharacter* DMGCauserCharacter = Cast<ABaseCharacter>(DamageCauser);
	if (nullptr != DMGCauserCharacter)
	{
		//UE_LOG(LogTemp, Warning, TEXT("sword attack"));
		int m_ftype = static_cast<int>(DamageEvent.DamageTypeClass.GetDefaultObject()->DamageFalloff);
		Network::GetNetwork()->send_hitmyself_packet(s_socket, DMGCauserCharacter->c_id, m_ftype);
		UE_LOG(LogTemp, Warning, TEXT("sword attack, Take Damage : NotifyHit %d"), m_ftype);
	}

	return Damage;
}

bool AAICharacter::ConnServer()
{
	Super::ConnServer();
	s_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
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
	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), recv_Aicallback);
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

void AAICharacter::recvPacket()
{
	Super::recvPacket();
	DWORD recv_flag = 0;
	ZeroMemory(&recv_expover.getWsaOver(), sizeof(recv_expover.getWsaOver()));

	recv_expover.getWsaBuf().buf = reinterpret_cast<char*>(recv_expover.getBuf() + _prev_size);
	recv_expover.getWsaBuf().len = BUFSIZE - _prev_size;

	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), recv_Aicallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}