// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"
#include "Inventory.h"
#include "Network.h"

ATree::ATree()
    : CanHarvest(true)
{
}


void ATree::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);


    if (OtherActor)
    {
        //AMyCharacter* player = Cast<AMyCharacter>(OtherActor);
        //if (player)
        //{
        //    player->OverlapInteract = true;
        //    player->OverlapType = true;
        //    player->OverlapInteractId = TreeId;
        //}
    }
}

void ATree::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

    if (OtherActor)
    {
        //AMyCharacter* player = Cast<AMyCharacter>(OtherActor);
        //if (player)
        //{
        //    player->OverlapInteract = false;
        //    player->OverlapType = true;
        //    player->OverlapInteractId = 0;
        //}
    }
}

void ATree::GenerateFruit(int _FruitType)
{
    UWorld* world = GetWorld();
    if (world)
    {


        /*
        * Ai는 비주얼적인 요소 제거
        */
        //switch (_FruitType)
        //{
        //case 0:
        //{
        //
        //
        //    break;
        //}
        //}
        //FName path = AInventory::ItemCodeToItemFruitPath(_FruitType);
        ////FName path = TEXT("Blueprint'/Game/Objects/Fruit/FallinFruit_BP.FallinFruit_BP_C'");
        //UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
        //FActorSpawnParameters SpawnParams;
        //SpawnParams.Owner = this;
        //FRotator rotator;
        //FVector  SpawnLocation = GetActorLocation();
        //SpawnLocation.Z += 90.0f;
        //mFruitMesh[0] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
        //SpawnLocation.X -= 45.0f;
        //mFruitMesh[1] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
        //SpawnLocation.X += 90.0f;
        //mFruitMesh[2] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);

        CanHarvest = true;
    }
}


void ATree::HarvestFruit()
{
    /*
    * Ai는 비주얼적인 요소 제거
    */

    //for (auto& fruit : mFruitMesh)
    //{
    //    if (fruit != nullptr)
    //    {
    //        fruit->Destroy();
    //        fruit = nullptr;
    //    }
    //}

    CanHarvest = false;

}
// Called when the game starts or when spawned
void ATree::BeginPlay()
{
    Super::BeginPlay();
    {
        Network::GetNetwork()->mTree[TreeId] = this;

        UWorld* world = GetWorld();
        if (world)
        {
            //비주얼 요소 제거 

            //FName path = TEXT("Blueprint'/Game/Objects/Fruit/FallinFruit_BP.FallinFruit_BP_C'");
            //UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
            //FActorSpawnParameters SpawnParams;
            //SpawnParams.Owner = this;
            //FRotator rotator;
            //FVector  SpawnLocation = GetActorLocation();
            //SpawnLocation.Z += 90.0f;
            //mFruitMesh[0] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
            //SpawnLocation.X -= 45.0f;
            //mFruitMesh[1] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
            //SpawnLocation.X += 90.0f;
            //mFruitMesh[2] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
        }
    }
}

void ATree::EndPlay(EEndPlayReason::Type Reason)
{
    Network::GetNetwork()->mTree[TreeId] = nullptr;
}
