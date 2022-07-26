// Fill out your copyright notice in the Description page of Project Settings.


#include "HealSpawner.h"
#include "Fruit.h"
#include "MyCharacter.h"
#include "BaseCharacter.h"
#include "Inventory.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AHealSpawner::AHealSpawner()
    :HealSpanwerId(-1)
    ,CanHarvest(false)
{
}

void AHealSpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if (OtherActor)
	{
		ABaseCharacter* player = Cast<ABaseCharacter>(OtherActor);
		if (player && CanHarvest)
		{
            if (INVALID_SOCKET != player->s_socket)
            {
                CanHarvest = false;
                send_getfruits_healspawner_packet(player->s_socket, HealSpanwerId);

                player->bIsUndertheHealSpawner = true;

            }
		}
    }
}

void AHealSpawner::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

    if (OtherActor)
    {
        ABaseCharacter* player = Cast<ABaseCharacter>(OtherActor);
        if (player)
        {
            player->bIsUndertheHealSpawner = false;

        }
    }
}



void AHealSpawner::GenerateFruit(int _FruitType)
{
    UWorld* world = GetWorld();
    if (world)
    {
        if (false == CanHarvest)
        {
            FName path = AInventory::ItemCodeToItemFruitPath(_FruitType);
            //UE_LOG(LogTemp, Log, TEXT("HealSpawner Generate Fruit Type : %d"), _FruitType);
            UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            FRotator rotator{};
            FVector  SpawnLocation = GetActorLocation();
            SpawnLocation.Z += 40.0f;
            mFruitMesh = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
        }
        CanHarvest = true;
    }
}


void AHealSpawner::HarvestFruit()
{
    /*
	여기에 뭐 Harvest 되는 행동을 취하면 됨. 지금은 Destory로 단순히 없애기만함.
	*/
    UGameplayStatics::PlaySoundAtLocation(this, HealSound, GetActorLocation(), 0.25f);

	if (mFruitMesh != nullptr)
	{
		mFruitMesh->Destroy();
		mFruitMesh = nullptr;
	}

    CanHarvest = false;

}
// Called when the game starts or when spawned
void AHealSpawner::BeginPlay()
{
    Super::BeginPlay();
    Network::GetNetwork()->mHealSpawner[HealSpanwerId] = this;
}

void AHealSpawner::EndPlay(EEndPlayReason::Type Reason)
{
    Network::GetNetwork()->mPunnet[HealSpanwerId] = nullptr;
}
