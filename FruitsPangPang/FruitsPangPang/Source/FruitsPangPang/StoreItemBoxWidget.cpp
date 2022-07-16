// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreItemBoxWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Network.h"
#include "MyCharacter.h"

void UStoreItemBoxWidget::NativePreConstruct()
{
	BuyButton->OnClicked.AddDynamic(this, &UStoreItemBoxWidget::ClickBuy_0);
	EquipButton->OnClicked.AddDynamic(this, &UStoreItemBoxWidget::ClickEquip);
	EquipButton->SetIsEnabled(false);
}

void UStoreItemBoxWidget::ClickBuy_0()
{
	auto Character = Network::GetNetwork()->mMyCharacter;
	if (nullptr != Character)
	{
		if (Character->Cash >= mPrice)
		{
			send_buy_packet(Network::GetNetwork()->mMyCharacter->l_socket, mitemcode);
		}
		else {
			//µ· ¾ø´Ù´Â Ã¢ ¶ç¿öÁÙ ¿¹Á¤.
		}
	}
}

void UStoreItemBoxWidget::ClickEquip()
{
	send_equip_packet(Network::GetNetwork()->mMyCharacter->l_socket, mitemcode);
}

void UStoreItemBoxWidget::InitializeItem(const int& itemcode, const FText& name, const int& price)
{

	UTexture2D* icon;
	icon = ItemCodeToShopItemIcon(itemcode);
	if (nullptr != icon)
	{
		ItemImage->SetBrushFromTexture(icon);
#define LOCTEXT_NAMESPACE "price"
		PriceText->SetText(FText::Format(LOCTEXT("price", "{0}"), price));
#undef LOCTEXT_NAMESPACE
		ItemName->SetText(name);
		mPrice = price;
		mitemcode = itemcode;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Store Item Resource is Unvalid! Check Icon"));
	}
}

// 1:Knife 2:Candy 3: Leaf 4: Straw 5: SunFlower
UTexture2D* UStoreItemBoxWidget::ItemCodeToShopItemIcon(const int& itemCode)
{
	UTexture2D* res = nullptr;
	switch (itemCode)
	{
	case 1:
		res = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Widget/Assets/T_knife1.T_knife1'"), NULL, LOAD_None, NULL);
		break;
	case 2:
		res = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Widget/Assets/T_candy.T_candy'"), NULL, LOAD_None, NULL);
		break;
	case 3:
		res = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Widget/Assets/T_leaf1.T_leaf1'"), NULL, LOAD_None, NULL);
		break;
	case 4:
		res = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Widget/Assets/T_straw1.T_straw1'"), NULL, LOAD_None, NULL);
		break;
	case 5:
		res = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Widget/Assets/T_sunflower1.T_sunflower1'"), NULL, LOAD_None, NULL);
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	case 11:
		break;
	case 12:
		break;
	default:
		break;
	}
	return res;
}