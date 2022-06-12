// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BronzeIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBronzeIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativePreConstruct() override;

	virtual void NativeTick(const FGeometry& Geometry, float DeltaSeconds) override;

	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_0;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* SizeBox_0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* BronzeIcon;


	class UPointOfInterestWidget* mPOIWidget;

	bool bTickActive;
	bool isOn;

private:
	float FindAngle(FVector2D a, FVector2D b);
	FVector2D FindCoord(float radius, float degree);

	float mDimension;
	float mZoom;

	float playerLocX;
	float playerLocY;

	float ownerLocX;
	float ownerLocY;
};
