// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMapWidget.h"
#include "PointOfInterestWidget.h"
#include "PointOfInterestComponent.h"
#include "PlayerIconWidget.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Engine/Texture.h"


void UMiniMapWidget::AddPOI(AActor* actor)
{
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MiniMap/W_PointOfInterest.W_PointOfInterest_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		mPointOfInterestWidget = CreateWidget<UPointOfInterestWidget>(GetWorld(), WidgetClass);

		mPOIComponent = Cast<UPointOfInterestComponent>(actor->GetComponentByClass(UPointOfInterestComponent::StaticClass()));

		mPointOfInterestWidget->Owner = actor;
		mPointOfInterestWidget->isStatic = mPOIComponent->isStatic;
		mPointOfInterestWidget->isOn = mPOIComponent->isOn;
		mPointOfInterestWidget->isCharacter = mPOIComponent->isCharacter;

		auto poiWGT = MapOverlay->AddChildToOverlay(mPointOfInterestWidget);
		poiWGT->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		poiWGT->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
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

