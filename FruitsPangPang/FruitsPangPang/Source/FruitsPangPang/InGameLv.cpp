// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLv.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Blueprint/UserWidget.h"


void AInGameLv::BeginPlay() {

	/*
	
	1. Loading Widget을 띄우고
	2. Map에 먼저 로딩해야할것들을 넣음
	2-1. 캐릭터를 스폰하고 Network mMyCharacter에 연결
	3. 서버에 연결함
	4. 서버에서 login_Ok패킷을 받아서 나무 값들 로딩
	5. Map에서 로딩해야할것들을 빼면서 [로딩할 때 freeze가 걸리는 asset들 ex)Niagra System]  Loading Widget의 percent를 올림
	6. 로딩이 전부 끝나면 Loading End 패킷을 보냄
	7. 이후부터는 GameStart패킷을 받으면서 시작될 것.
	
	*/

	//FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MLoadingWidget.MLoadingWidget_C'"));
	//auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
	//if (nullptr == WidgetClass)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("MainWidget Source is invalid !! check '/Game/Widget/MLoadingWidget.MLoadingWidget_C'"));
	//	return;
	//}
	//UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	//LoadingWidget->AddToViewport();

	FName path = TEXT("Blueprint'/Game/Character/BP_MyCharacter.BP_MyCharacter_C'"); //_C를 꼭 붙여야 된다고 함.
	UClass* GeneratedCharacterBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform trans;
	//trans.SetLocation(trans.GetLocation() + FVector(0, 0, 300));
	//auto mc = GetWorld()->SpawnActorDeferred<AMyCharacter>(GeneratedInventoryBP, trans);
	//mc->SpawnDefaultController();
	//mc->AutoPossessPlayer = EAutoReceiveInput::Disabled;
	//mc->FinishSpawning(trans);
	
	trans.SetLocation(trans.GetLocation() + FVector(200, 0, 300));
	auto mc1 = GetWorld()->SpawnActorDeferred<AMyCharacter>(GeneratedCharacterBP, trans);
	mc1->SpawnDefaultController();
	mc1->AutoPossessPlayer = EAutoReceiveInput::Player0;
	mc1->GameState = 1;
	mc1->FinishSpawning(trans);
	CreateLoadingWidget();
	Conn();

	//To Loading ...

	//To Do SomeThing ...

	//Loading Complete ! Send Complete Packet
	send_PreGameSettingComplete_packet(Network::GetNetwork()->mMyCharacter->s_socket);

}

void AInGameLv::Conn()
{
	Network::GetNetwork()->init();
	auto player = Network::GetNetwork()->mMyCharacter;
	if (nullptr != player)
	{
		player->ConnServer();
		send_login_packet(player->s_socket, 0);
		UE_LOG(LogTemp, Log, TEXT("Player Try Conn"));
	}
	UE_LOG(LogTemp, Log, TEXT("Begin Played"));
}

void AInGameLv::CreateLoadingWidget()
{

	if(nullptr != Network::GetNetwork()->mMyCharacter)
		Network::GetNetwork()->mMyCharacter->MakeLoadingHUD();

}