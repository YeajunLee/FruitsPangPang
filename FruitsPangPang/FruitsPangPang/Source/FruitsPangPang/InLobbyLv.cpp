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

	//FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MLoadingWidget.MLoadingWidget_C'"));
	//auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
	//if (nullptr == WidgetClass)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("MainWidget Source is invalid !! check '/Game/Widget/MLoadingWidget.MLoadingWidget_C'"));
	//	return;
	//}
	//UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	//LoadingWidget->AddToViewport();

	FName path = TEXT("Blueprint'/Game/Character/BP_MyCharacter.BP_MyCharacter_C'"); //_C�� �� �ٿ��� �ȴٰ� ��.
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