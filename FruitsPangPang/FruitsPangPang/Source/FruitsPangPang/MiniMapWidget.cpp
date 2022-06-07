// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMapWidget.h"
#include "PointOfInterestWidget.h"
#include "PointOfInterestComponent.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Engine/Texture.h"


void UMiniMapWidget::AddPOI(AActor* actor)
{
	FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MiniMap/W_PointOfInterest.W_PointOfInterest_C'"));
	auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
	mPointOfInterestWidget = CreateWidget<UPointOfInterestWidget>(GetWorld(), WidgetClass);

	mPOIComponent = Cast<UPointOfInterestComponent>( actor->GetComponentByClass(UPointOfInterestComponent::StaticClass()));
	
	mPointOfInterestWidget->Owner = actor;
	mPointOfInterestWidget->isStatic = mPOIComponent->isStatic;

	//mPOIComponent->isStatic =  mPointOfInterestWidget->isStatic;
	
	MapOverlay->AddChildToOverlay(mPointOfInterestWidget)->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	MapOverlay->AddChildToOverlay(mPointOfInterestWidget)->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	
}
