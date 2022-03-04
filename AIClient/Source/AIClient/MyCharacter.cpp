// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Projectile.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Math/Quat.h"
#include "Kismet/KismetMathLibrary.h"
#include "Network.h"

// Sets default values
AMyCharacter::AMyCharacter()
	:s_connected(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeLocation(FVector(-70.f, 0.f, 0.f));
	FollowCamera->bUsePawnControlRotation = false;

	//Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//GroundSpeed = 0.f;

	GetCharacterMovement()->bOrientRotationToMovement = true; //캐릭터 방향에 따라
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1080.f, 0.f); // 회전
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto p = GetController();
	if (p != nullptr)
	{
		if (!p->IsPlayerController())
		{
			UE_LOG(LogTemp, Error, TEXT("WorldCnt : %d"), Network::GetNetwork()->WorldCharacterCnt);
			Network::GetNetwork()->mOtherCharacter[Network::GetNetwork()->WorldCharacterCnt] = this;
			Network::GetNetwork()->WorldCharacterCnt++;
		}
	}

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		float PitchClamp = FMath::ClampAngle(Rotation.Pitch, -45.f, 45.f);
		FRotator RotationControl(PitchClamp, Rotation.Yaw, Rotation.Roll);


		Controller->SetControlRotation(RotationControl);
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMyCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMyCharacter::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::AddControllerPitchInput);

}

void AMyCharacter::MoveForward(float value)
{
	if (Controller != nullptr && value != 0.f)
	{


		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AMyCharacter::MoveRight(float value)
{
	if (Controller != nullptr && value != 0.f)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);

	}
}

void AMyCharacter::TurnAtRate(float rate)
{
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::LookUpAtRate(float rate)
{
	AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyCharacter::LMBDown()
{
	bLMBDown = true;

	Attack();
}

void AMyCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMyCharacter::Attack()
{
	if (!bAttacking)
	{
		bAttacking = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ThrowMontage)
		{
			AnimInstance->Montage_Play(ThrowMontage, 2.f);
			AnimInstance->Montage_JumpToSection(FName("Default"), ThrowMontage);

		}
	}
}

void AMyCharacter::AttackEnd()
{
	bAttacking = false;
	if (bLMBDown)
	{
		Attack();
	}
}


void AMyCharacter::Jump()
{
	Super::Jump();
}

void AMyCharacter::Throw()
{
	//FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");
	FVector SocketLocation = GetMesh()->GetSocketLocation("BombSocket");

	FRotator SpawnActorRotation = FollowCamera->GetComponentRotation();


	FName path = TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb.Bomb_C'");
	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	//GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketTransform);
	GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketLocation, SpawnActorRotation);

	FVector FowardVector = UKismetMathLibrary::GetForwardVector(SpawnActorRotation) * 1000.f;
	
	//UPrimitiveComponent* PhysicsSetter = nullptr;
	//SetPhysicsLinearVelocity(SocketForward);

}



void AMyCharacter::Throw(const FVector& location,FRotator rotation,const FName& path)
{
	rotation.Pitch += 18;
	FTransform trans(rotation.Quaternion(), location);


	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	//GetWorld()->SpawnActor<AProjectile>(GeneratedBP, SocketTransform);
	auto bomb = GetWorld()->SpawnActor<AProjectile>(GeneratedBP, trans);
	//FAttachmentTransformRules attachrules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
	//bomb->AttachToComponent(this->GetMesh(), attachrules, "BombSocket");
	//FDetachmentTransformRules Detachrules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative,true);
	//bomb->DetachFromActor(Detachrules);
	bomb->ProjectileMovementComponent->Activate();
}