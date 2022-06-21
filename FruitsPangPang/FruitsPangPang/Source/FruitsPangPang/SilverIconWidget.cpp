// Fill out your copyright notice in the Description page of Project Settings.


#include "SilverIconWidget.h"
#include "PointOfInterestWidget.h"
#include "ScoreWidget.h"
#include "MainWidget.h"
#include "MiniMapWidget.h"
#include "MyCharacter.h"
#include "Network.h"
#include "Inventory.h"
#include "Components/Image.h"
#include "Math/Vector2D.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"

void USilverIconWidget::NativePreConstruct()
{
	bTickActive = false;
}

void USilverIconWidget::NativeTick(const FGeometry& Geometry, float DeltaSeconds)
{

	mZoom = Network::GetNetwork()->mMyCharacter->mMainWidget->W_MiniMap_0->Zoom;
	mDimension = Network::GetNetwork()->mMyCharacter->mMainWidget->W_MiniMap_0->Dimension;

	playerLocX = Network::GetNetwork()->mMyCharacter->GetActorLocation().X;
	playerLocY = Network::GetNetwork()->mMyCharacter->GetActorLocation().Y;


	if (bTickActive)
	{
		if (Network::GetNetwork()->mMyCharacter == Network::GetNetwork()->mMyCharacter->mInventory->mMainWidget->mScoreWidget->ScoreBoard[0].GetCharacter())
		{
			SilverIcon->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			SilverIcon->SetVisibility(ESlateVisibility::Visible);
			ownerLocX = Network::GetNetwork()->mMyCharacter->mInventory->mMainWidget->mScoreWidget->ScoreBoard[1].GetCharacter()->GetActorLocation().X;
			ownerLocY = Network::GetNetwork()->mMyCharacter->mInventory->mMainWidget->mScoreWidget->ScoreBoard[1].GetCharacter()->GetActorLocation().Y;

			{
				SetRenderTranslation(FindCoord(
					FVector2D((
						playerLocX - ownerLocX) / ((mDimension / 350) * mZoom), (playerLocY - ownerLocY) * (-1) / ((mDimension / 350) * mZoom))
					.Size(),
					FindAngle(FVector2D(0, 0), FVector2D((
						playerLocX - ownerLocX) / ((mDimension / 350) * mZoom), (playerLocY - ownerLocY) * (-1) / ((mDimension / 350) * mZoom)))
				));

			}
		}
	}

}

float USilverIconWidget::FindAngle(FVector2D a, FVector2D b)
{
	return atan2(a.Y - b.Y, a.X - b.X);
}

FVector2D USilverIconWidget::FindCoord(float radius, float degree)
{
	return FVector2D(FMath::Sin(degree) * FMath::Clamp(radius, 0.0f, 165.f) * (-1), FMath::Cos(degree) * FMath::Clamp(radius, 0.0f, 165.f) * (-1));
}
