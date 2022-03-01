// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "AIController_Custom.h"
#include "Projectile.h"
#include "Tree.h"
#include "Punnet.h"
#include "Inventory.h"

// Sets default values
AAICharacter::AAICharacter()
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
	mInventory->mCharacter = this;	// ExposeOnSpawn하고 SpawnActor에서 값 넣어주는게 C++로 짜면 이런식 인듯
	mInventory->mAmountOfSlots = 5;
	mInventory->FinishSpawning(spawnLocAndRot);
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		if (c_id == Network::GetNetwork()->mId) {
			Network::GetNetwork()->send_anim_packet(Network::AnimType::Throw);
			Network::GetNetwork()->send_useitem_packet(SelectedHotKeySlotNum, 1);
		}
		AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ThrowMontage_AI)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack!"));

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

void AAICharacter::Throw_AI()
{
	FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");
	SocketTransform.GetRotation();
	SocketTransform.GetLocation();
	SocketTransform.GetScale3D();
	//FName path = TEXT("Blueprint'/Game/Bomb/Bomb.Bomb_C'"); //_C를 꼭 붙여야 된다고 함.
	//FName path = TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb.Bomb_C'");
	FName path = AInventory::ItemCodeToItemBombPath(SavedHotKeyItemCode);

	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	AProjectile* bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketTransform);	
	bomb->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "BombSocket");

	Network::GetNetwork()->send_spawnobj_packet(SocketTransform.GetLocation(), SocketTransform.GetRotation(), SocketTransform.GetScale3D(), SavedHotKeyItemCode);
}

void AAICharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackEnd.Broadcast();
} 

void AAICharacter::GetFruits()
{
	if (OverlapType)
	{
		Network::GetNetwork()->mTree[OverlapInteractId]->CanHarvest = false;
		Network::GetNetwork()->send_getfruits_tree_packet(OverlapInteractId);
		UE_LOG(LogTemp, Log, TEXT("Tree Fruit"));
	}
	else {
		Network::GetNetwork()->mPunnet[OverlapInteractId]->CanHarvest = false;
		Network::GetNetwork()->send_getfruits_punnet_packet(OverlapInteractId);
		UE_LOG(LogTemp, Log, TEXT("Punnet Fruit"));
	}
}



void AAICharacter::ConnServer()
{
	s_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	//char server_ad[30] = "127.0.0.1";
	//cout << "접속할 ip를 입력하세요:";
	//cin.getline(server_ad, 30);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	int rt = WSAConnect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), NULL, NULL, NULL, NULL);
	//if (SOCKET_ERROR == rt)
	//{
	//	std::cout << "connet Error :";
	//	int err_num = WSAGetLastError();
	//	error_display(err_num);
	//	cout << "connection eliminate." << endl;
	//	//system("pause");
	//	//exit(0);
	//	return false;
	//}
	//return true;

	//1들어간자리에 ai Numbering 해주면 된다.
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(s_socket), hiocp, 1, 0);

	DWORD recv_flag = 0;
	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), NULL);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}

void AAICharacter::recvPacket()
{
	DWORD recv_flag = 0;
	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), NULL);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}