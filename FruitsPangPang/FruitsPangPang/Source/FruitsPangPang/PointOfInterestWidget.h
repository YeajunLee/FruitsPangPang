// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PointOfInterestWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UPointOfInterestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& Geometry, float DeltaSeconds) override;

	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay0;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* SizeBox0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* CustomImage;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class AActor* Owner;
	bool isStatic;
	bool isOn;

	float FindAngle(FVector2D a, FVector2D b);
	FVector2D FindCoord(float radius, float degree);
private:
	float mDimension;
	float mZoom;

	float playerLocX;
	float playerLocY;

	float ownerLocX;
	float ownerLocY;

};
