// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PointOfInterestComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRUITSPANGPANG_API UPointOfInterestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPointOfInterestComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		bool isStatic;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Actor Image")
		class UTexture2D* iconImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Image")
		class UTexture2D* Enemy1Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Image")
		class UTexture2D* Enemy2Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Image")
		class UTexture2D* Enemy3Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isOn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isCharacter;
	

	void InitializePOI();

};
