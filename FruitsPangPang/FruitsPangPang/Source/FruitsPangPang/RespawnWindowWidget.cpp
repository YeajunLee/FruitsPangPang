// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnWindowWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "RespawnWidget.h"

void URespawnWindowWidget::NativePreConstruct()
{


	RemainTime = 5.0f;
#define LOCTEXT_NAMESPACE "respawn"
	RemainTimeText->SetText(FText::Format(LOCTEXT("respawn","Select Respawn Area\nremain time : {0}s"), static_cast<int>(RespawnTime)));
#undef LOCTEXT_NAMESPACE
	RemainTimeText->SetJustification(ETextJustify::Type::Center);
	auto slotbox = MinimapBox->AddChildToVerticalBox(RemainTimeText);
	slotbox->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	slotbox->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);


	FSoftClassPath my(TEXT("Blueprint'/Game/Widget/MRespawnWidget.MRespawnWidget_C'"));
	auto p = my.TryLoadClass<UUserWidget>();
	auto wi = CreateWidget<URespawnWidget>(GetWorld(), p);
	MinimapBox->AddChildToVerticalBox(wi);
	mRespawnWidget = wi;

	bActivate = true;
}


void URespawnWindowWidget::NativeDestruct()
{

}


void URespawnWindowWidget::NativeTick(const FGeometry& my, float deltatime)
{
	if (!bActivate) return;

	RemainTime = FMath::Max<float>(RemainTime - deltatime, 0.0f);

#define LOCTEXT_NAMESPACE "respawn"
	RemainTimeText->SetText(FText::Format(LOCTEXT("respawn", "Select Respawn Area\nremain time :{0}s"), static_cast<int>(RemainTime)));
#undef LOCTEXT_NAMESPACE

}