// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"


// Sets default values
AMyCharacter::AMyCharacter()
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
	GetCharacterMovement()->JumpZVelocity = 420.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetController()->IsPlayerController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("other id ")));
		Network::GetNetwork()->mMyCharacter = this;
		Network::GetNetwork()->init();
		Network::GetNetwork()->C_Recv();
		Network::GetNetwork()->send_login_packet();
	}
	else {
		Network::GetNetwork()->mOtherCharacter[Network::GetNetwork()->WorldCharacterCnt] = this;
		Network::GetNetwork()->WorldCharacterCnt++;
	}
	//if (Network::GetNetwork()->WorldCharacterCnt == 0)
	//{
	//	if (GetController()->IsPlayerController())
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	//			FString::Printf(TEXT("other id ")));
	//	}
	//	Network::GetNetwork()->mMyCharacter = this;
	//	Network::GetNetwork()->init();
	//	Network::GetNetwork()->C_Recv();
	//	Network::GetNetwork()->send_login_packet();
	//	Network::GetNetwork()->WorldCharacterCnt++;
	//}
	//else {
	//	if (GetController()->IsPlayerController())
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	//			FString::Printf(TEXT("other id ")));
	//	}
	//	auto tm = Network::GetNetwork()->WorldCharacterCnt;
	//	//내 자신이 1 증가시키고 그다음부턴 others에 들어가게
	//	Network::GetNetwork()->mOtherCharacter[Network::GetNetwork()->WorldCharacterCnt - 1] = this;
	//	Network::GetNetwork()->WorldCharacterCnt++;
	//}
	//GetMesh()->SetVisibility(false);
}

void AMyCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	Network::GetNetwork()->release();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		float PitchClamp = FMath::ClampAngle(Rotation.Pitch, -20.f, 30.f);
		FRotator RotationControl(PitchClamp, Rotation.Yaw, Rotation.Roll);

		if (GetController()->IsPlayerController()) {
			auto pos = GetTransform().GetLocation();
			auto rot = GetTransform().GetRotation();
			Network::GetNetwork()->send_move_packet(pos.X, pos.Y, pos.Z, rot, GroundSpeedd, MOVE_RIGHT);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			//	FString::Printf(TEXT("MY id : My pos:%f,%f,%f , value : "), pos.X, pos.Y, pos.Z));
		}
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		//	FString::Printf(TEXT("char before : %f,%f,%f"), this->GetTransform().GetLocation().X, GetTransform().GetLocation().Y, GetTransform().GetLocation().Z));
		auto a = GetTransform().GetLocation().Y;
		Controller->SetControlRotation(RotationControl);
		if (a != GetTransform().GetLocation().Y)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			//	FString::Printf(TEXT("char after : %f,%f,%f"), this->GetTransform().GetLocation().X, GetTransform().GetLocation().Y, GetTransform().GetLocation().Z));

		}
	}

	
	
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent); //플레이어 입력 구성 요소가 유효한지 확인하는 매크로

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMyCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMyCharacter::LMBUp);


	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);


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

		//auto pos = GetTransform().GetLocation();
		//auto rot = GetTransform().GetRotation();
		//
		//Network::GetNetwork()->send_move_packet(pos.X,pos.Y,pos.Z,rot, GroundSpeed,MOVE_FORWARD);
		
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		//	FString::Printf(TEXT("after x y z : %f %f %f "), pos.X, pos.Y, pos.Z));
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
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		//	FString::Printf(TEXT("My pos:%f,%f,%f , value : %f "), Direction.X, Direction.Y, Direction.Z, value));
		
		//SetActorLocation(GetTransform().GetLocation() * 0.1);
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
	
	//UE_LOG(LogTemp, Log, TEXT("lmbdown"));

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
		if (GetController()->IsPlayerController()) {
			Network::GetNetwork()->send_anim_packet(Network::AnimType::Throw);
		}
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

void AMyCharacter::Throww()
{
	FTransform SocketTransform = GetMesh()->GetSocketTransform("BombSocket");

	FName path = TEXT("Blueprint'/Game/Bomb/Bomb.Bomb_C'"); //_C를 꼭 붙여야 된다고 함.
	UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	auto bomb = GetWorld()->SpawnActor<AActor>(GeneratedBP, SocketTransform);
	//Network::GetNetwork()->send_anim_packet(Network::AnimType::Throw);
	//spawnActor함수 사용법 모르겠음 일단 skip->는 개뿔 줫밥새끼

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	//	FString::Printf(TEXT("My pos: ")));

	
}
