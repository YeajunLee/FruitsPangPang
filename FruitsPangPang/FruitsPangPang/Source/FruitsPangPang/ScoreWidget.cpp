// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"
#include <algorithm>
#include "Network.h"
#include "MyCharacter.h"
#include "Components/TextBlock.h"

using namespace std;

ScoreInfo::ScoreInfo()
	:mCharacter(nullptr)
	,score(0)
{

}

ScoreInfo::ScoreInfo(class ABaseCharacter* character)
	:mCharacter(character)
	,score(mCharacter->score)
{

}

void ScoreInfo::UpdateScore()
{
	score = (mCharacter->killcount * 2) - mCharacter->deathcount;
}

void UScoreWidget::NativePreConstruct()
{

#define LOCTEXT_NAMESPACE "score"
	No_1->SetText(FText::FromString("No.1"));
	No_2->SetText(FText::FromString("No.2"));
	No_3->SetText(FText::FromString("No.3"));
	No_4->SetText(FText::FromString("No.4"));
	No_5->SetText(FText::FromString("No.5"));
	No_6->SetText(FText::FromString("No.6"));
	No_7->SetText(FText::FromString("No.7"));
	No_8->SetText(FText::FromString("No.8"));

	if(nullptr!= Network::GetNetwork()->mMyCharacter)
		ScoreBoard.push_back(ScoreInfo(Network::GetNetwork()->mMyCharacter));
	//for (int i = 0; i < 8; ++i)
	//{
	//	if (nullptr != Network::GetNetwork()->mOtherCharacter[i])
	//		ScoreBoard.push_back(ScoreInfo(Network::GetNetwork()->mOtherCharacter[i]));
	//}
	UpdateRank();



#undef LOCTEXT_NAMESPACE

}

void UScoreWidget::NativeDestruct()
{
	ScoreBoard.clear();
}

void UScoreWidget::UpdateScoreBoard()
{
	for (auto& scoreInfo : ScoreBoard)
	{
		scoreInfo.UpdateScore();
	}
}

void UScoreWidget::UpdateRank()
{
	UpdateScoreBoard();
	sort(ScoreBoard.begin(), ScoreBoard.end());


#define LOCTEXT_NAMESPACE "score"
	for (int i = 0; i < ScoreBoard.size(); ++i)
	{
		switch (i)
		{
		case 0:
			//No_1Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_1Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ScoreBoard[i].GetCharacter()->c_id));
			No_1Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 1:
			No_2Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ScoreBoard[i].GetCharacter()->c_id));
			No_2Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 2:
			No_3Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ScoreBoard[i].GetCharacter()->c_id));
			No_3Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 3:
			No_4Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ScoreBoard[i].GetCharacter()->c_id));
			No_4Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 4:
			No_5Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ScoreBoard[i].GetCharacter()->c_id));
			No_5Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 5:
			No_6Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ScoreBoard[i].GetCharacter()->c_id));
			No_6Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 6:
			No_7Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ScoreBoard[i].GetCharacter()->c_id));
			No_7Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 7:
			No_8Name->SetText(FText::Format(LOCTEXT("score", "ID:{0}"), ScoreBoard[i].GetCharacter()->c_id));
			No_8Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;

		}
	}
	for (int i = ScoreBoard.size(); i < MAX_USER; ++i)
	{
		switch (i)
		{
		case 0:
			No_1Name->SetText(FText::FromString("ID:NoName"));
			No_1Score->SetText(FText::FromString("(-)"));
			break;
		case 1:
			No_2Name->SetText(FText::FromString("ID:NoName"));
			No_2Score->SetText(FText::FromString("(-)"));
			break;
		case 2:
			No_3Name->SetText(FText::FromString("ID:NoName"));
			No_3Score->SetText(FText::FromString("(-)"));
			break;
		case 3:
			No_4Name->SetText(FText::FromString("ID:NoName"));
			No_4Score->SetText(FText::FromString("(-)"));
			break;
		case 4:
			No_5Name->SetText(FText::FromString("ID:NoName"));
			No_5Score->SetText(FText::FromString("(-)"));
			break;
		case 5:
			No_6Name->SetText(FText::FromString("ID:NoName"));
			No_6Score->SetText(FText::FromString("(-)"));
			break;
		case 6:
			No_7Name->SetText(FText::FromString("ID:NoName"));
			No_7Score->SetText(FText::FromString("(-)"));
			break;
		case 7:
			No_8Name->SetText(FText::FromString("ID:NoName"));
			No_8Score->SetText(FText::FromString("(-)"));
			break;

		}
	}
#undef LOCTEXT_NAMESPACE
}