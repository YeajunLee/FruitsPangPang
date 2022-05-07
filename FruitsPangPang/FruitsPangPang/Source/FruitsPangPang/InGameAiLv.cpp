// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameAiLv.h"
#include "Network.h"
#include "AICharacter.h"


void AInGameAiLv::BeginPlay() {

	ConnAi();
}

void AInGameAiLv::ConnAi()
{
	Network::GetNetwork()->init();
	FName path = TEXT("Blueprint'/Game/Character/AICharacter/BP_AICharacter.BP_AICharacter_C'"); //_C를 꼭 붙여야 된다고 함.
	UClass* GeneratedCharacterBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform trans;
	trans.SetLocation(FVector(23660, 10195, 5610));
	if (ACTIVE_AI_CNT > 0)
	{
		
		for (int i = 0; i < ACTIVE_AI_CNT; ++i)
		{
			trans.SetLocation(trans.GetLocation() + FVector(200, 0, 0));
			auto mc1 = GetWorld()->SpawnActorDeferred<AAICharacter>(GeneratedCharacterBP, trans);
			mc1->AutoPossessPlayer = EAutoReceiveInput::Disabled;
			mc1->bIsDie = true;
			mc1->FinishSpawning(trans);
			Network::GetNetwork()->send_PreGameSettingComplete_packet(mc1->s_socket);
		}
	}
	//int i = 0;
	//for (auto ai : Network::GetNetwork()->mAiCharacter)
	//{
	//	if (nullptr != ai)
	//	{
	//		ai->ConnServer();
	//		Network::GetNetwork()->send_login_packet(ai->s_socket, 1);
	//		UE_LOG(LogTemp, Log, TEXT("Ai Number :%d Try Conn"), i);
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	//			FString::Printf(TEXT("server conn")));
	//	}
	//	++i;
	//}

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	//	FString::Printf(TEXT("server conn")));
	//UE_LOG(LogTemp, Log, TEXT("Begin Played"));
}