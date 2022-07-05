// Fill out your copyright notice in the Description page of Project Settings.


#include "InLobbyLv.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Blueprint/UserWidget.h"

void AInLobbyLv::BeginPlay() {

	Network::GetNetwork()->bLevelOpenTriggerEnabled = false;	//레벨 시작됐으니 트리거 꺼줌.
	/*

	1. 캐릭터를 스폰하고 Network mMyCharacter에 연결
	2. 서버에 연결함
	3. 로그인 ui
	4. 로그인 성공시 ui닫히고 로비 시작. 
	*/
	FName path = TEXT("Blueprint'/Game/Character/BP_MyCharacter.BP_MyCharacter_C'"); //_C를 꼭 붙여야 된다고 함.
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
		if(!Network::GetNetwork()->bLoginFlag)
			player->ShowLoginHUD();
		else
		{
			const char* tmpid = TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterName);
			const char* tmppass = TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterPassWord);
			if (nullptr != Network::GetNetwork()->mMyCharacter)
				send_login_lobby_packet(Network::GetNetwork()->mMyCharacter->l_socket, tmpid, tmppass);
		}
		UE_LOG(LogTemp, Log, TEXT("Player Try Conn"));

	}
	UE_LOG(LogTemp, Log, TEXT("Begin Played"));
}