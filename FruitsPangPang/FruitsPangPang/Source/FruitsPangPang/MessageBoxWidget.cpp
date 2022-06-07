// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageBoxWidget.h"
#include "Components/TextBlock.h"

void UMessageBoxWidget::NativeDestruct()
{
	MakeMessageBoxDelegate.Clear();
}



void UMessageBoxWidget::MakeMessageBox(const FString& msg)
{
	mMsg->SetText(FText::FromString(msg));
}


void UMessageBoxWidget::MakeMessageBoxWithCode(const int& msgCode)
{
	if (MakeMessageBoxDelegate.IsBound() == true)
		MakeMessageBoxDelegate.Broadcast(msgCode);
}