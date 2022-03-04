// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLv.h"
#include "Network.h"
#include "AICharacter.h"

void AInGameLv::BeginPlay() {

	ConnAi();
}

void AInGameLv::ConnAi()
{
	Network::GetNetwork()->init();
	int i = 0;
	for (auto ai : Network::GetNetwork()->mAiCharacter)
	{
		if (nullptr != ai)
		{
			ai->ConnServer();
			Network::GetNetwork()->send_login_packet(ai->s_socket);
			UE_LOG(LogTemp, Log, TEXT("Ai Number :%d Try Conn"), i);
		}
		++i;
	}
	UE_LOG(LogTemp, Log, TEXT("Begin Played"));
}