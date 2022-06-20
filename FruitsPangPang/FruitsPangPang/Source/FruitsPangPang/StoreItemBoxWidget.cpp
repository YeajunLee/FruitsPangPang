// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemBoxWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Network.h"
#include "MyCharacter.h"

void UStoreItemBoxWidget::NativePreConstruct()
{
	BuyBotton->OnClicked.AddDynamic(this, &UStoreItemBoxWidget::ClickBuy_0);
	
}void UStoreItemBoxWidget::ClickBuy_0()
{
	//Network::GetNetwork()->mMyCharacter->bNifeSkin = true;
}
