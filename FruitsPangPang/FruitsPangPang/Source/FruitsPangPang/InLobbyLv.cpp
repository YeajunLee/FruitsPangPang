// Fill out your copyright notice in the Description page of Project Settings.


#include "InLobbyLv.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Blueprint/UserWidget.h"

void AInLobbyLv::BeginPlay() {

	/*

	1. ĳ���͸� �����ϰ� Network mMyCharacter�� ����
	2. ������ ������
	3. �α��� ui
	4. �α��� ������ ui������ �κ� ����. 
	*/
	FName path = TEXT("Blueprint'/Game/Character/BP_MyCharacter.BP_MyCharacter_C'"); //_C�� �� �ٿ��� �ȴٰ� ��.
	UClass* GeneratedCharacterBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform trans;
	trans.SetLocation(trans.GetLocation() + FVector(200, 0, 300));
	auto mc1 = GetWorld()->SpawnActorDeferred<AMyCharacter>(GeneratedCharacterBP, trans);
	mc1->SpawnDefaultController();
	mc1->AutoPossessPlayer = EAutoReceiveInput::Player0;
	mc1->GameState = 0;
	mc1->FinishSpawning(trans);
	Conn();

	//To Loading ...

	//To Do SomeThing ...

}

void AInLobbyLv::Conn()
{
	Network::GetNetwork()->init();
	auto player = Network::GetNetwork()->mMyCharacter;
	if (nullptr != player)
	{
		player->ConnLobbyServer();
		player->ShowLoginHUD();
		//send_login_packet(player->s_socket, 0);
		UE_LOG(LogTemp, Log, TEXT("Player Try Conn"));

	}
	UE_LOG(LogTemp, Log, TEXT("Begin Played"));
}