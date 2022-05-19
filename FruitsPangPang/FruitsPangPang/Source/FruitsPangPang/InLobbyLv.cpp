// Fill out your copyright notice in the Description page of Project Settings.


#include "InLobbyLv.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Blueprint/UserWidget.h"

void AInLobbyLv::BeginPlay() {

	/*

	1. 캐릭터를 스폰하고 Network mMyCharacter에 연결
	2. 서버에 연결함
	3. 로그인 ui
	4. 로그인 성공시 ui닫히고 로비 시작. 
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
	//mc1->mLoadingWidget = LoadingWidget;
	mc1->FinishSpawning(trans);
	Conn();

	//To Loading ...

	//To Do SomeThing ...

	//Loading Complete ! Send Complete Packet
	send_PreGameSettingComplete_packet(Network::GetNetwork()->mMyCharacter->s_socket);

}

void AInLobbyLv::Conn()
{
	Network::GetNetwork()->init();
	auto player = Network::GetNetwork()->mMyCharacter;
	if (nullptr != player)
	{
		player->ConnLobbyServer();
		send_login_packet(player->s_socket, 0);
		UE_LOG(LogTemp, Log, TEXT("Player Try Conn"));
	}
	UE_LOG(LogTemp, Log, TEXT("Begin Played"));
}