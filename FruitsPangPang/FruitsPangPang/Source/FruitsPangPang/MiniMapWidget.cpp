// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMapWidget.h"
#include "MyCharacter.h"
#include "PointOfInterestWidget.h"
#include "PointOfInterestComponent.h"
#include "PlayerIconWidget.h"
#include "GoldIconWidget.h"
#include "SilverIconWidget.h"
#include "BronzeIconWidget.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Engine/Texture.h"


void UMiniMapWidget::AddPOI(AActor* actor)
{
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MiniMap/W_PointOfInterest.W_PointOfInterest_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		
		auto mmPointOfInterestWidget = CreateWidget<UPointOfInterestWidget>(GetWorld(), WidgetClass);

		mPOIComponent = Cast<UPointOfInterestComponent>(actor->GetComponentByClass(UPointOfInterestComponent::StaticClass()));

		mmPointOfInterestWidget->Owner = actor;
		mmPointOfInterestWidget->isStatic = mPOIComponent->isStatic;


		auto poiWGT = MapOverlay->AddChildToOverlay(mmPointOfInterestWidget);
		poiWGT->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		poiWGT->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MiniMap/W_GoldIcon.W_GoldIcon_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		mGoldIconWidget = CreateWidget<UGoldIconWidget>(GetWorld(), WidgetClass);

		auto iconWGT = MapOverlay->AddChildToOverlay(mGoldIconWidget);
		iconWGT->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		iconWGT->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MiniMap/W_SilverIcon.W_SilverIcon_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		mSilverIconWidget = CreateWidget<USilverIconWidget>(GetWorld(), WidgetClass);

		auto iconWGT = MapOverlay->AddChildToOverlay(mSilverIconWidget);
		iconWGT->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		iconWGT->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MiniMap/W_BronzeIcon.W_BronzeIcon_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		mBronzeIconWidget = CreateWidget<UBronzeIconWidget>(GetWorld(), WidgetClass);

		auto iconWGT = MapOverlay->AddChildToOverlay(mBronzeIconWidget);
		iconWGT->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		iconWGT->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MiniMap/W_PlayerIcon.W_PlayerIcon_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		mPlayerIconWidget = CreateWidget<UPlayerIconWidget>(GetWorld(), WidgetClass);

		auto iconWGT = MapOverlay->AddChildToOverlay(mPlayerIconWidget);
		iconWGT->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		iconWGT->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
}

