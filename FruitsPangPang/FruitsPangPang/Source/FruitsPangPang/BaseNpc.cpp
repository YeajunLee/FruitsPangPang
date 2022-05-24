// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseNpc.h"
#include "MyCharacter.h"
#include "BaseCharacter.h"
#include "AICharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABaseNpc::ABaseNpc()
{
    //현재 Tick이 돌 필요가 없어서 false. Tick을 써야하면 이곳을 켜주자.
	PrimaryActorTick.bCanEverTick = false;

    mStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mStaticMesh"));
    mStaticMesh->SetupAttachment(GetRootComponent());

    mBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("mBoxCollision"));
    mBoxCollision->SetupAttachment(mStaticMesh);

    mBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseNpc::OverlapNpcBoxBegin);
    mBoxCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseNpc::OverlapNpcBoxEnd);
}


void ABaseNpc::OverlapNpcBoxBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        ABaseCharacter* player = Cast<ABaseCharacter>(OtherActor);
        if (player)
        {
            player->OverlapInteract = true;
            player->OverlapInteractId = NpcCode;
        }
    }
}

void ABaseNpc::OverlapNpcBoxEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

    if (OtherActor)
    {
        ABaseCharacter* player = Cast<ABaseCharacter>(OtherActor);
        if (player)
        {
            player->OverlapInteract = false;
            player->OverlapInteractId = -1;
        }
    }
}

// Called when the game starts or when spawned
void ABaseNpc::BeginPlay()
{
    Super::BeginPlay();
	
}

// Called every frame
void ABaseNpc::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //현재는 Tick이 안돌아가는 상태.
    // 쓸 일이 있다면 생성자부분의
    // PrimaryActorTick.bCanEverTick를 true로 바꿔주자
    //
}
