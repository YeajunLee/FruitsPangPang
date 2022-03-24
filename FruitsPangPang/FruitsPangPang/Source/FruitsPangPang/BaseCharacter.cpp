// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
	:hp(PLAYER_HP)
	,SelectedHotKeySlotNum(0)
	,SavedHotKeyItemCode(0)
	,mInventory(nullptr)
	,OverlapInteract(false)
	,OverlapInteractId(-1)
	,OverlapType(true)
	,_prev_size(0)
	,c_id(-1)
	,overID(-1)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BaseCharacter"));

	bIsUndertheTree = false;
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::GetFruits()
{

}

bool ABaseCharacter::ConnServer()
{
	return true;
}

void ABaseCharacter::recvPacket()
{
	
}