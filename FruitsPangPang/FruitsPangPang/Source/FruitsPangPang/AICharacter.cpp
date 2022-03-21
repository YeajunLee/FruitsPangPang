// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "AIControllerCustom.h"
#include "Inventory.h"
#include "Projectile.h"
#include "Tree.h"
#include "Punnet.h"
#include "Network.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIControllerCustom::StaticClass();
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
