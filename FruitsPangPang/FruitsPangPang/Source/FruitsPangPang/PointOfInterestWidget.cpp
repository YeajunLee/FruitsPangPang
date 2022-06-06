// Fill out your copyright notice in the Description page of Project Settings.


#include "PointOfInterestWidget.h"
#include "PointOfInterestComponent.h"
#include "MainWidget.h"
#include "MiniMapWidget.h"
#include "Network.h"
#include "MyCharacter.h"
#include "ScoreWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Math/Vector2D.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"

void UPointOfInterestWidget::NativePreConstruct()
{
	bTickActive = false;
}


void UPointOfInterestWidget::NativeTick(const FGeometry& Geometry, float DeltaSeconds)
{
	Super::NativeTick(Geometry, DeltaSeconds);


	//if (bTickActive)
	//{
	//	score0_id = Network::GetNetwork()->mMyCharacter->mScoreWidget->ScoreBoard[0].GetCharacter()->c_id;
	//	enemyLocX = Network::GetNetwork()->mMyCharacter->mScoreWidget->ScoreBoard[0].GetCharacter()->GetActorLocation().X;
	//	enemyLocY = Network::GetNetwork()->mMyCharacter->mScoreWidget->ScoreBoard[0].GetCharacter()->GetActorLocation().Y;
	//}
	
	
	mZoom = Network::GetNetwork()->mMyCharacter->mMainWidget->W_MiniMap_0->Zoom;
	mDimension = Network::GetNetwork()->mMyCharacter->mMainWidget->W_MiniMap_0->Dimension;

	playerLocX = Network::GetNetwork()->mMyCharacter->GetActorLocation().X;
	playerLocY = Network::GetNetwork()->mMyCharacter->GetActorLocation().Y;

	ownerLocX = Owner->GetActorLocation().X;
	ownerLocY = Owner->GetActorLocation().Y;

	isOn = Network::GetNetwork()->mMyCharacter->POIcomponent->isOn;

	// dimension 을 Main widget에서 W_Minimap의 사이즈(350)으로 나누는 것임

	//CustomImage2->SetVisibility(ESlateVisibility::Hidden);
	if (!isCharacter)
	{

		SetRenderTranslation(FindCoord(
			FVector2D((
				playerLocX - ownerLocX) / ((mDimension / 350) * mZoom), (playerLocY - ownerLocY) * (-1) / ((mDimension / 350) * mZoom))
			.Size(),
			FindAngle(FVector2D(0, 0), FVector2D((
				playerLocX - ownerLocX) / ((mDimension / 350) * mZoom), (playerLocY - ownerLocY) * (-1) / ((mDimension / 350) * mZoom)))
		));

		if (isStatic == false)
		{
			if (FVector2D((
				playerLocX - ownerLocX) / ((mDimension / 350) * mZoom), (playerLocY - ownerLocY) * (-1) / ((mDimension / 350) * mZoom))
				.Size() > 165)
			{
				ActorImage->SetVisibility(ESlateVisibility::Hidden);
			}
			else
				ActorImage->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if(bTickActive)
	{
		score0_id = Network::GetNetwork()->mMyCharacter->mScoreWidget->ScoreBoard[0].GetCharacter()->c_id;
		enemyLocX = Network::GetNetwork()->mMyCharacter->mScoreWidget->ScoreBoard[0].GetCharacter()->GetActorLocation().X;
		enemyLocY = Network::GetNetwork()->mMyCharacter->mScoreWidget->ScoreBoard[0].GetCharacter()->GetActorLocation().Y;
		if (isCharacter)
		{
			{

				SetRenderTranslation(FindCoord(
					FVector2D((
						playerLocX - enemyLocX) / ((mDimension / 350) * mZoom), (playerLocY - enemyLocY) * (-1) / ((mDimension / 350) * mZoom))
					.Size(),
					FindAngle(FVector2D(0, 0), FVector2D((
						playerLocX - enemyLocX) / ((mDimension / 350) * mZoom), (playerLocY - enemyLocY) * (-1) / ((mDimension / 350) * mZoom)))
				));

				if (isStatic == false)
				{
					if (FVector2D((
						playerLocX - enemyLocX) / ((mDimension / 350) * mZoom), (playerLocY - enemyLocY) * (-1) / ((mDimension / 350) * mZoom))
						.Size() > 165)
					{
						CharacterImage1->SetVisibility(ESlateVisibility::Hidden);
					}
					else
						CharacterImage1->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
		
	}
	if (!bTickActive)
		return;

	UE_LOG(LogTemp, Log, TEXT("%f"),enemyLocX);

		/*if (isOn == false)
		{
			if(Cast<AMyCharacter>(Owner))
				CharacterImage1->SetVisibility(ESlateVisibility::Hidden);
		}*/

		//if (isOn == true)
		//{
		//	if (Cast<AMyCharacter>(Owner))
		//	CharacterImage1->SetVisibility(ESlateVisibility::Visible);
		//}
		
}

float UPointOfInterestWidget::FindAngle(FVector2D a, FVector2D b)
{
	
	return atan2(a.Y- b.Y, a.X - b.X);
}

FVector2D UPointOfInterestWidget::FindCoord(float radius, float degree)
{
	return FVector2D(FMath::Sin(degree) * FMath::Clamp(radius, 0.0f, 165.f) * (-1) ,FMath::Cos(degree) * FMath::Clamp(radius, 0.0f, 165.f)*(-1) );
}
