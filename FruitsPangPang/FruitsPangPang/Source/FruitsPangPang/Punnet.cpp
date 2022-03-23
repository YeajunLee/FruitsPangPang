// Fill out your copyright notice in the Description page of Project Settings.


#include "Punnet.h"
#include "Fruit.h"
#include "MyCharacter.h"
#include "BaseCharacter.h"
#include "Inventory.h"

APunnet::APunnet()
    : CanHarvest(true)
{
}

void APunnet::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);


    if (OtherActor)
    {
        ABaseCharacter* player = Cast<ABaseCharacter>(OtherActor);
        if (player)
        {
            player->OverlapInteract = true;
            player->OverlapInteractId = PunnetId;
            player->OverlapType = false;
        }
    }
}

void APunnet::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

    if (OtherActor)
    {
        ABaseCharacter* player = Cast<ABaseCharacter>(OtherActor);
        if (player)
        {
            player->OverlapInteract = false;
            player->OverlapInteractId = 0;
            player->OverlapType = false;
        }
    }
}



void APunnet::GenerateFruit(int _FruitType)
{
    UWorld* world = GetWorld();
    if (world)
    {
        switch (_FruitType)
        {
        case 0:
        {


            break;
        }
        }
        // 일단 임시로 모든 과일 다 토마토로 작업.
        FName path = AInventory::ItemCodeToItemFruitPath(_FruitType);
        UE_LOG(LogTemp, Log, TEXT("Punnet Generate Fruit Type : %d"), _FruitType);
        //FName path = TEXT("Blueprint'/Game/Objects/Fruit/FallinFruit_BP.FallinFruit_BP_C'");
        UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        FRotator rotator;
        FVector  SpawnLocation = GetActorLocation();
        SpawnLocation.Z += 10.0f;
        mFruitMesh[0] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
        SpawnLocation.X -= 20.0f;
        mFruitMesh[1] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
        SpawnLocation.X += 20.0f;
        mFruitMesh[2] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);

        CanHarvest = true;
    }
}


void APunnet::HarvestFruit()
{
    /*
    여기에 뭐 Harvest 되는 행동을 취하면 됨. 지금은 Destory로 단순히 없애기만함.
    */
    for (auto& fruit : mFruitMesh)
    {
        if (fruit != nullptr)
        {
            fruit->Destroy();
            fruit = nullptr;
        }
    }

    CanHarvest = false;

}
// Called when the game starts or when spawned
void APunnet::BeginPlay()
{
    Super::BeginPlay();
    Network::GetNetwork()->mPunnet[PunnetId] = this;    
}

void APunnet::EndPlay(EEndPlayReason::Type Reason)
{
    Network::GetNetwork()->mPunnet[PunnetId] = nullptr;
}
