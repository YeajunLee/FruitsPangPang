// Fill out your copyright notice in the Description page of Project Settings.


#include "GameResultWidget.h"
#include "ScoreWidget.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


void UGameResultWidget::NativePreConstruct()
{

	FInputModeUIOnly gamemode;
	auto controller = GetWorld()->GetFirstPlayerController();
	if (nullptr != controller)
	{
		controller->SetInputMode(gamemode);
		controller->SetShowMouseCursor(true);
	}
	LobbyButton->OnClicked.AddDynamic(this, &UGameResultWidget::GoToLobby);

	mTextBlockRank.push_back(No_1);
	mTextBlockRank.push_back(No_2);
	mTextBlockRank.push_back(No_3);
	mTextBlockRank.push_back(No_4);
	mTextBlockRank.push_back(No_5);
	mTextBlockRank.push_back(No_6);
	mTextBlockRank.push_back(No_7);
	mTextBlockRank.push_back(No_8);

	mTextBlockName.push_back(No_1Name);
	mTextBlockName.push_back(No_2Name);
	mTextBlockName.push_back(No_3Name);
	mTextBlockName.push_back(No_4Name);
	mTextBlockName.push_back(No_5Name);
	mTextBlockName.push_back(No_6Name);
	mTextBlockName.push_back(No_7Name);
	mTextBlockName.push_back(No_8Name);

	mTextBlockScore.push_back(No_1Score);
	mTextBlockScore.push_back(No_2Score);
	mTextBlockScore.push_back(No_3Score);
	mTextBlockScore.push_back(No_4Score);
	mTextBlockScore.push_back(No_5Score);
	mTextBlockScore.push_back(No_6Score);
	mTextBlockScore.push_back(No_7Score);
	mTextBlockScore.push_back(No_8Score);


	if (nullptr == mScoreWidget)
		return;
	auto ResultBoard = mScoreWidget->ScoreBoard;
	int myid = Network::GetNetwork()->mMyCharacter->c_id;

#define LOCTEXT_NAMESPACE "score"
	for (int i = 0; i < mTextBlockRank.size(); ++i)
	{
		mTextBlockRank[i]->SetText(FText::Format(LOCTEXT("score", "No.{0}"), i + 1));
	}

	for (int i = 0; i < ResultBoard.size(); ++i)
	{
		if (i == myid)
		{
			mTextBlockRank[i]->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 0)));
			mTextBlockName[i]->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 0)));
			mTextBlockScore[i]->SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 0)));
			mTextBlockName[i]->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ResultBoard[i].GetCharacter()->c_id));
			mTextBlockScore[i]->SetText(FText::Format(LOCTEXT("score", "({0})"), ResultBoard[i].GetScore()));
		}
		else {
			mTextBlockName[i]->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ResultBoard[i].GetCharacter()->c_id));
			mTextBlockScore[i]->SetText(FText::Format(LOCTEXT("score", "({0})"), ResultBoard[i].GetScore()));
		}
	}

	for (int i = ResultBoard.size(); i < MAX_USER; ++i)
	{
		mTextBlockName[i]->SetText(FText::FromString("-"));
		mTextBlockScore[i]->SetText(FText::FromString("(-)"));		
	}

#undef LOCTEXT_NAMESPACE
}


void UGameResultWidget::NativeDestruct()
{

	mTextBlockRank.clear();
	mTextBlockName.clear();
	mTextBlockScore.clear();
 }


void UGameResultWidget::GoToLobby()
{
	FGenericPlatformMisc::RequestExit(false);
}