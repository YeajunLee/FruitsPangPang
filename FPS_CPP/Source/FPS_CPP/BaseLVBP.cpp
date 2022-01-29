// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseLVBP.h"
#include "Network.h"
#include "MyCharacter.h"



void ABaseLVBP::Conn(UPARAM(ref) ACharacter* charRef)
{
	Network::GetNetwork()->mMyCharacter = Cast<AMyCharacter>(charRef);
	if (Network::GetNetwork()->init())
	{
		Network::GetNetwork()->C_Recv();
		Network::GetNetwork()->send_login_packet();
	}
}


void ABaseLVBP::DisConn()
{

	Network::GetNetwork()->release();
}