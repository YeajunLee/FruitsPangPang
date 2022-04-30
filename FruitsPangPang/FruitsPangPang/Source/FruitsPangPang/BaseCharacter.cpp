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
	,killcount(0),deathcount(0),score(0)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GreenOnionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GreenOnionMesh"));
	GreenOnionMesh->SetupAttachment(GetRootComponent());
	ConstructorHelpers::FObjectFinder<UStaticMesh> GreenOnionAsset(TEXT("/Game/Assets/Fruits/BigGreenOnion/SM_GreenOnion.SM_GreenOnion"));
	if (GreenOnionAsset.Succeeded())
		GreenOnionMesh->SetStaticMesh(GreenOnionAsset.Object);
	
	CarrotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrotMesh"));
	CarrotMesh->SetupAttachment(GetRootComponent());
	ConstructorHelpers::FObjectFinder<UStaticMesh> CarrotAsset(TEXT("/Game/Assets/Fruits/Carrot/SM_Carrot.SM_Carrot"));
	if (CarrotAsset.Succeeded())
		CarrotMesh->SetStaticMesh(CarrotAsset.Object);

	GreenOnionMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::OnOverlapBegin);
	GreenOnionMesh->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::OnOverlapEnd);

	CarrotMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::OnOverlapBegin);
	CarrotMesh->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::OnOverlapEnd);
	
	bIsDie = false;
	bStepBanana = false;
	bHitbyFruit = false;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BaseCharacter"));

	bIsUndertheTree = false;

	GreenOnionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarrotMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GreenOnionMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GreenOnionSocket"));
	CarrotMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("CarrotSocket"));
	
	GreenOnionMesh->SetHiddenInGame(true, false);
	CarrotMesh->SetHiddenInGame(true, false);
	
	GreenOnionMesh->SetGenerateOverlapEvents(true);
	CarrotMesh->SetGenerateOverlapEvents(true);
	GreenOnionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CarrotMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
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

void ABaseCharacter::Throw()
{

}
//void ABaseCharacter::AttackEnd()
//{
//
//}

void ABaseCharacter::GetFruits()
{

}


void ABaseCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ABaseCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
bool ABaseCharacter::ConnServer()
{
	return true;
}

void ABaseCharacter::recvPacket()
{
	
}