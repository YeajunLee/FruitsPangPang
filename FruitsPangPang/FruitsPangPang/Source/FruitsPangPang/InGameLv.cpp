// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLv.h"

#include "Network.h"
#include "MyCharacter.h"


void AInGameLv::BeginPlay() {

	Conn();
}

void AInGameLv::Conn()
{
	Network::GetNetwork()->init();
	auto player = Network::GetNetwork()->mMyCharacter;
	if (nullptr != player)
	{
		player->ConnServer();
		Network::GetNetwork()->send_login_packet(player->s_socket);
		UE_LOG(LogTemp, Log, TEXT("Player Try Conn"));
	}
	UE_LOG(LogTemp, Log, TEXT("Begin Played"));
}