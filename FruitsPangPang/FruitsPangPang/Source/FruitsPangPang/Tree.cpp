// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"
#include "Fruit.h"
#include "MyCharacter.h"
#include "BaseCharacter.h"
#include "Inventory.h"
#include "AICharacter.h"
#include "PointOfInterestComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ATree::ATree()
    : CanHarvest(false)
{
    POIcomp = CreateDefaultSubobject<UPointOfInterestComponent>(TEXT("POIComp"));
}


void ATree::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);


    if (OtherActor)
    {
        ABaseCharacter* player = Cast<ABaseCharacter>(OtherActor);
        if (player)
        {
            player->OverlapInteract = true;
            player->OverlapType = true;
            player->OverlapInteractId = TreeId;

            player->bIsUndertheTree = true;
        }
    }
}

void ATree::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

    if (OtherActor)
    {
        ABaseCharacter* player = Cast<ABaseCharacter>(OtherActor);
        if (player)
        {
            player->OverlapInteract = false;
            player->OverlapType = true;
            player->OverlapInteractId = -1;

            player->bIsUndertheTree = false;
           // UE_LOG(LogTemp, Error, TEXT("[EndOverlap] player Type : %d , treeID : %d"), player->c_id, TreeId);
        }
    }
}

void ATree::GenerateFruit(int _FruitType)
{
    UWorld* world = GetWorld();
    if (world)
    {
        if (false == CanHarvest)
        {
          FName path = AInventory::ItemCodeToItemFruitPath(_FruitType);
          UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
          FActorSpawnParameters SpawnParams;
          SpawnParams.Owner = this;
          FRotator rotator;
          FVector  SpawnLocation = GetActorLocation();
          SpawnLocation.Z += 100.0f;
          SpawnLocation.Y += 25.0f;
          SpawnLocation.X += 30.0f;
          mFruitMesh[0] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
          SpawnLocation.X -= 45.0f;
          SpawnLocation.Y -= 30.0f;
          mFruitMesh[1] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
          SpawnLocation.X += 80.0f;
          mFruitMesh[2] = world->SpawnActor<AFruit>(GeneratedBP, SpawnLocation, rotator, SpawnParams);
        }
        CanHarvest = true;
    }
}


void ATree::HarvestFruit()
{
    /*
    여기에 뭐 Harvest 되는 행동을 취하면 됨. 지금은 Destory로 단순히 없애기만함.
    */
    if(TreeId >= 0 && TreeId <= 41) //0~41 초록나무, 42~55 주황나무
        UGameplayStatics::PlaySoundAtLocation(this, HarvestTreeSound, GetActorLocation(), 0.7f);
    else if(TreeId >= 42 && TreeId <= 55)
        UGameplayStatics::PlaySoundAtLocation(this, HarvestTreeSound2, GetActorLocation(), 0.6f);

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
void ATree::BeginPlay()
{
    Super::BeginPlay();
    {
        Network::GetNetwork()->mTree[TreeId] = this;
    }
}

void ATree::EndPlay(EEndPlayReason::Type Reason)
{
    Network::GetNetwork()->mTree[TreeId] = nullptr;
}
