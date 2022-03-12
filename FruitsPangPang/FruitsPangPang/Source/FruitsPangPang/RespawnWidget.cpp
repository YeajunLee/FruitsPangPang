// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnWidget.h"
#include "Components/Button.h"
#include "Network.h"

void URespawnWidget::NativePreConstruct()
{
	Button0_0->OnClicked.AddDynamic(this, &URespawnWidget::BClick0_0);
	Button1_0->OnClicked.AddDynamic(this, &URespawnWidget::BClick1_0);
	Button2_0->OnClicked.AddDynamic(this, &URespawnWidget::BClick2_0);
	Button0_1->OnClicked.AddDynamic(this, &URespawnWidget::BClick0_1);
	Button1_1->OnClicked.AddDynamic(this, &URespawnWidget::BClick1_1);
	Button2_1->OnClicked.AddDynamic(this, &URespawnWidget::BClick2_1);
	Button0_2->OnClicked.AddDynamic(this, &URespawnWidget::BClick0_2);
	Button1_2->OnClicked.AddDynamic(this, &URespawnWidget::BClick1_2);
	Button2_2->OnClicked.AddDynamic(this, &URespawnWidget::BClick2_2);
}


void URespawnWidget::NativeDestruct()
{

}



void URespawnWidget::BClick0_0()
{
	Network::GetNetwork()->send_respawn_packet(0);
	UE_LOG(LogTemp, Log, TEXT("0,0"));
}

void URespawnWidget::BClick1_0()
{
	Network::GetNetwork()->send_respawn_packet(1);
	UE_LOG(LogTemp, Log, TEXT("1,0"));
}

void URespawnWidget::BClick2_0()
{

	Network::GetNetwork()->send_respawn_packet(2);
	UE_LOG(LogTemp, Log, TEXT("2,0"));
}

void URespawnWidget::BClick0_1()
{

	Network::GetNetwork()->send_respawn_packet(3);
	UE_LOG(LogTemp, Log, TEXT("0,1"));
}

void URespawnWidget::BClick1_1()
{

	Network::GetNetwork()->send_respawn_packet(8);
	UE_LOG(LogTemp, Log, TEXT("1,1"));
}

void URespawnWidget::BClick2_1()
{

	Network::GetNetwork()->send_respawn_packet(4);
	UE_LOG(LogTemp, Log, TEXT("2,1"));
}

void URespawnWidget::BClick0_2()
{

	Network::GetNetwork()->send_respawn_packet(5);
	UE_LOG(LogTemp, Log, TEXT("0,2"));
}

void URespawnWidget::BClick1_2()
{

	Network::GetNetwork()->send_respawn_packet(6);
	UE_LOG(LogTemp, Log, TEXT("1,2"));
}

void URespawnWidget::BClick2_2()
{

	Network::GetNetwork()->send_respawn_packet(7);
	UE_LOG(LogTemp, Log, TEXT("2,2"));
}