// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
	:hp(PLAYER_HP)
	, SelectedHotKeySlotNum(0)
	, SavedHotKeySlotNum(0)
	, mInventory(nullptr)
	, OverlapInteract(false)
	, OverlapInteractId(-1)
	, OverlapType(true)
	, _prev_size(0)
	, c_id(-1)
	, overID(-1)
	, killcount(0), deathcount(0), score(0)
	, bIsDie(false)
	, bStepBanana(false)
	, bHitbyFruit(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SM_GreenOnion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_GreenOnion"));
	SM_GreenOnion->SetupAttachment(GetRootComponent());


	SM_Carrot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Carrot"));
	SM_Carrot->SetupAttachment(GetRootComponent());

	SM_GreenOnion->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::GreenOnionBeginOverlap);
	SM_GreenOnion->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::GreenOnionEndOverlap);

	SM_Carrot->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::CarrotBeginOverlap);
	SM_Carrot->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::CarrotEndOverlap);

	
}

void ABaseCharacter::GreenOnionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseCharacter::GreenOnionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ABaseCharacter::CarrotBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseCharacter::CarrotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("BaseCharacter"));

	bIsUndertheTree = false;

	//SM_GreenOnion->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SM_Carrot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SM_GreenOnion->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GreenOnionSocket"));
	SM_Carrot->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("CarrotSocket"));
	
	SM_GreenOnion->SetHiddenInGame(true, false);
	SM_Carrot->SetHiddenInGame(true, false);
	
	SM_GreenOnion->SetGenerateOverlapEvents(true);
	SM_Carrot->SetGenerateOverlapEvents(true);
	SM_GreenOnion->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SM_Carrot->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
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

bool ABaseCharacter::ConnServer()
{
	return true;
}

void ABaseCharacter::recvPacket()
{
	
}