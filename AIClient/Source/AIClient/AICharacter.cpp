// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "AIController_Custom.h"
#include "Projectile.h"

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController_Custom::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //레벨에 배치하거나 새로 생성되는 AI는 AIConstrollerCustom의 지배를 받게된다.
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AAICharacter::Attack()
{
	if (!bAttacking)
	{
		bAttacking = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ThrowMontage_AI)
		{
			AnimInstance->Montage_Play(ThrowMontage_AI, 2.f);
			AnimInstance->Montage_JumpToSection(FName("Default"), ThrowMontage_AI);

		}
	}
}

void AAICharacter::AttackEnd()
{
	bAttacking = false;
	Attack();
}

void AAICharacter::Throw_AI()
{
	FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");
	SocketTransform.GetRotation();
	SocketTransform.GetLocation();
	SocketTransform.GetScale3D();
	//FName path = TEXT("Blueprint'/Game/Bomb/Bomb.Bomb_C'"); //_C를 꼭 붙여야 된다고 함.
	FName path = TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb_Test.Bomb_Test_C'");
	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	auto bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketTransform);
	

}