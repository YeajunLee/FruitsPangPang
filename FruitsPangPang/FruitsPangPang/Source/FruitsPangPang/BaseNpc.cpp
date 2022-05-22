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
    //���� Tick�� �� �ʿ䰡 ��� false. Tick�� ����ϸ� �̰��� ������.
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

    //����� Tick�� �ȵ��ư��� ����.
    // �� ���� �ִٸ� �����ںκ���
    // PrimaryActorTick.bCanEverTick�� true�� �ٲ�����
    //
}
