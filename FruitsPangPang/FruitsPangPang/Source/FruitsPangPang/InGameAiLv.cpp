// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameAiLv.h"
#include "Network.h"
#include "AICharacter.h"


void AInGameAiLv::BeginPlay() {

	Network::GetNetwork()->bLevelOpenTriggerEnabled = false;	//���� ���۵����� Ʈ���� ����.
	ConnAi();
}

void AInGameAiLv::ConnAi()
{
	Network::GetNetwork()->init();
	FName path = TEXT("Blueprint'/Game/Character/AICharacter/BP_AICharacter.BP_AICharacter_C'"); //_C�� �� �ٿ��� �ȴٰ� ��.
	FName path2 = TEXT("Blueprint'/Game/Character/AICharacter/SwordAI/BP_SwordAI.BP_SwordAI_C'");
	FName path3 = TEXT("Blueprint'/Game/Character/AICharacter/SmartAI/BP_SmartAI.BP_SmartAI_C'");

	UClass* GeneratedCharacterBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	UClass* GeneratedCharacterBP2 = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path2.ToString()));
	UClass* GeneratedCharacterBP3 = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path3.ToString()));

	FTransform trans;
	trans.SetLocation(FVector(23660, 10195, 5610));
	int GenerateAiAmount = 0;
	if (Network::GetNetwork()->mAiAmount > 0)
		GenerateAiAmount = Network::GetNetwork()->mAiAmount;
	else
		GenerateAiAmount = ACTIVE_AI_CNT;
	for (int i = 0; i < GenerateAiAmount; ++i)
	{
		trans.SetLocation(trans.GetLocation() + FVector(200, 0, 0));
		AAICharacter* mc1;
		if (i == 0)
			mc1 = GetWorld()->SpawnActorDeferred<AAICharacter>(GeneratedCharacterBP2, trans);
		else if (i == 1)
			mc1 = GetWorld()->SpawnActorDeferred<AAICharacter>(GeneratedCharacterBP3, trans);
		else
			mc1 = GetWorld()->SpawnActorDeferred<AAICharacter>(GeneratedCharacterBP, trans);

		mc1->AutoPossessPlayer = EAutoReceiveInput::Disabled;
		mc1->bIsDie = true;
		mc1->FinishSpawning(trans);
		send_PreGameSettingComplete_packet(mc1->s_socket);
	}
}