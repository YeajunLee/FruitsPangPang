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

// Sets default values
AAICharacter::AAICharacter()
	:_prev_size(0)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController_Custom::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //레벨에 배치하거나 새로 생성되는 AI는 AIConstrollerCustom의 지배를 받게된다.
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

	FName path = TEXT("Blueprint'/Game/Inventory/Inventory_BP.Inventory_BP_C'"); //_C를 꼭 붙여야 된다고 함.
	UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform spawnLocAndRot{ GetActorLocation() };
	mInventory = GetWorld()->SpawnActorDeferred<AInventory>(GeneratedInventoryBP, spawnLocAndRot);
	mInventory->mOwnerCharacter = this;	// ExposeOnSpawn하고 SpawnActor에서 값 넣어주는게 C++로 짜면 이런식 인듯
	mInventory->mAmountOfSlots = 5;
	mInventory->FinishSpawning(spawnLocAndRot);

	/*
	* 토마토 5개 들고 시작
	*/
	//FItemInfo itemClass;
	//itemClass.ItemCode = 1;
	//itemClass.IndexOfHotKeySlot = 0;
	//itemClass.Name = AInventory::ItemCodeToItemName(1);
	//itemClass.Icon = AInventory::ItemCodeToItemIcon(1);
	//mInventory->UpdateInventorySlot(itemClass, 5);
	SelectedHotKeySlotNum = 0;

	overID = Network::GetNetwork()->getNewId();
	UE_LOG(LogTemp, Log, TEXT("Ai Number :%d Genereate"), overID);
	Network::GetNetwork()->mAiCharacter[overID] = this;
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
	/*if (!bAttacking)
	{
		bAttacking = true;
	}*/

	//Play Throw Montage	
	if (mInventory->mSlots[SelectedHotKeySlotNum].Amount > 0)
	{
		SavedHotKeyItemCode = mInventory->mSlots[SelectedHotKeySlotNum].ItemClass.ItemCode;
		mInventory->RemoveItemAtSlotIndex(SelectedHotKeySlotNum, 1);
		//if (c_id == Network::GetNetwork()->mId) 
		{
			Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Throw);
			Network::GetNetwork()->send_useitem_packet(s_socket, SelectedHotKeySlotNum, 1);
		}
		AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ThrowMontage_AI)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Attack!"));
			UE_LOG(LogTemp, Warning, TEXT("left tomato: %d"), mInventory->mSlots[SelectedHotKeySlotNum].Amount)

			AnimInstance->Montage_Play(ThrowMontage_AI, 2.f);
			AnimInstance->Montage_JumpToSection(FName("Default"), ThrowMontage_AI);
		}

		//에러가 계속 나서 AddDynamic을 AddUniqueDynamic으로 바꿈.
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
	//Ai가 바라보고 있는 방향벡터로 Rotater를 만든다음 그걸 소켓과 합쳐서 transform을 만든다.
	FRotator aiRotate = GetActorForwardVector().Rotation();
	FTransform trans(aiRotate.Quaternion(), SocketTransform.GetLocation());
	//Network::GetNetwork()->send_spawnobj_packet(s_socket, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), SocketTransform.GetScale3D(), SavedHotKeyItemCode);
	Network::GetNetwork()->send_spawnobj_packet(s_socket, SocketTransform.GetLocation(), aiRotate, SocketTransform.GetScale3D(), SavedHotKeyItemCode);

	FName path = AInventory::ItemCodeToItemBombPath(SavedHotKeyItemCode);

	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
	bomb->BombOwner = this;
	bomb->ProjectileMovementComponent->Activate();


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
		Network::GetNetwork()->mTree[OverlapInteractId]->CanHarvest = false;
		Network::GetNetwork()->send_getfruits_tree_packet(s_socket, OverlapInteractId);
		UE_LOG(LogTemp, Log, TEXT("Tree Fruit"));
	}
	else {
		//Network::GetNetwork()->mPunnet[OverlapInteractId]->CanHarvest = false;
		//Network::GetNetwork()->send_getfruits_punnet_packet(s_socket,OverlapInteractId);
		//UE_LOG(LogTemp, Log, TEXT("Punnet Fruit"));
	}
}



float AAICharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

bool AAICharacter::ConnServer()
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
