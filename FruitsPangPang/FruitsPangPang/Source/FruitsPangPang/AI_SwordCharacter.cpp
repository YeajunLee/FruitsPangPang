// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_SwordCharacter.h"
#include "AI_Sword_Controller_Custom.h"
#include "Inventory.h"
#include "Projectile.h"
#include "Tree.h"
#include "Punnet.h"
#include "Network.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h "
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

//PunnetInfo::PunnetInfo()
//	:mPunnet(nullptr)
//	,bIgnored(false)
//{
//
//}
//
//PunnetInfo::PunnetInfo(APunnet* punnet)
//	:mPunnet(punnet)
//	,bIgnored(false)
//{
//
//}


// Sets default values
AAI_SwordCharacter::AAI_SwordCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAI_Sword_Controller_Custom::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; //레벨에 배치하거나 새로 생성되는 AI는 AIConstrollerCustom의 지배를 받게된다.

	BananaCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BananaCollision"));;
	BananaCollision->SetupAttachment(GetMesh());
	BananaCollision->SetRelativeLocation(FVector(0.f, 0.f, -130.f));
	BananaCollision->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.4f));
	BananaCollision->SetMobility(EComponentMobility::Movable);
	BananaCollision->OnComponentBeginOverlap.AddDynamic(this, &AAI_SwordCharacter::OnBananaBoxOverlapBegin);


}

// Called when the game starts or when spawned
void AAI_SwordCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*overID = Network::GetNetwork()->getNewId();
	Network::GetNetwork()->mAISwordCharacter[overID] = this;
	Network::GetNetwork()->init();
	ConnServer();
	Network::GetNetwork()->send_login_packet(s_socket, 1);*/

	FName path = TEXT("Blueprint'/Game/Inventory/Inventory_BP.Inventory_BP_C'"); //_C를 꼭 붙여야 된다고 함.
	UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform spawnLocAndRot{ GetActorLocation() };
	mInventory = GetWorld()->SpawnActorDeferred<AInventory>(GeneratedInventoryBP, spawnLocAndRot);
	mInventory->mOwnerCharacter = this;	// ExposeOnSpawn하고 SpawnActor에서 값 넣어주는게 C++로 짜면 이런식 인듯
	mInventory->mAmountOfSlots = 5;
	mInventory->FinishSpawning(spawnLocAndRot);

	
}

void AAI_SwordCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	closesocket(s_socket);
	Network::GetNetwork()->release();
}

// Called every frame
void AAI_SwordCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();
	GroundSpeed_AI = CharXYVelocity;
}

// Called to bind functionality to input
void AAI_SwordCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//칼질
void AAI_SwordCharacter::Attack()
{
	if (!bAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwordAttack!"));
		AnimInstance = GetMesh()->GetAnimInstance();

		bAttacking = true;
		SavedHotKeySlotNum = SelectedHotKeySlotNum;

		if (AnimInstance && StabMontage_AI)
		{
			AnimInstance->Montage_Play(StabMontage_AI, 1.7f);
			AnimInstance->Montage_JumpToSection(FName("Default"), StabMontage_AI);
			Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Stab);

		}

		/*if (mInventory->mSlots[SelectedHotKeySlotNum].Amount > 0)
		{
			

			int HotKeyItemCode = mInventory->mSlots[SelectedHotKeySlotNum].ItemClass.ItemCode;

			if (HotKeyItemCode == 7) {
				if (AnimInstance && SlashMontage_AI)
				{
					AnimInstance->Montage_Play(SlashMontage_AI, 1.5f);
					AnimInstance->Montage_JumpToSection(FName("Default"), SlashMontage_AI);
					Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Slash);

				}
			}
			else if (HotKeyItemCode == 8) {
				if (AnimInstance && StabMontage_AI)
				{
					AnimInstance->Montage_Play(StabMontage_AI, 1.2f);
					AnimInstance->Montage_JumpToSection(FName("Default"), StabMontage_AI);
					Network::GetNetwork()->send_anim_packet(s_socket, Network::AnimType::Stab);

				}
			}
		}*/
		AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AAI_SwordCharacter::OnAttackMontageEnded);

	}
}
void AAI_SwordCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackEnd.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("OnAttackEnd~"));
}

void AAI_SwordCharacter::GetFruits()
{
	Super::GetFruits();
	if (OverlapType)
	{
		if (OverlapInteractId != -1)
		{
			Network::GetNetwork()->mTree[OverlapInteractId]->CanHarvest = false;
			Network::GetNetwork()->send_getfruits_tree_packet(s_socket, OverlapInteractId);
			//UE_LOG(LogTemp, Log, TEXT("Tree Fruit"));
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Overlap is -1 But Try GetFruits - Type:Tree"));
		}
	}
	else {
		if (OverlapInteractId != -1)
		{
			Network::GetNetwork()->mPunnet[OverlapInteractId]->CanHarvest = false;
			Network::GetNetwork()->send_getfruits_punnet_packet(s_socket, OverlapInteractId);
			//UE_LOG(LogTemp, Log, TEXT("Punnet Fruit"));
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Overlap is -1 But Try GetFruits - Type:Punnet"));
		}
	}
}

void AAI_SwordCharacter::OnTimeEnd()
{
	bStepBanana = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AAI_SwordCharacter::OnBananaBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor)
	{
		AProjectile* banana = Cast<AProjectile>(OtherActor);
		if (nullptr != banana)
		{
			if (banana->_fType == 11)
			{
				bStepBanana = true;
				banana->Destroy();
				GetCharacterMovement()->DisableMovement();
				/*if (P_Star1 && P_Star1->Template)
				{
					P_Star1->ToggleActive();
				}
				UGameplayStatics::PlaySoundAtLocation(this, dizzySound1, GetActorLocation());*/

				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAI_SwordCharacter::OnTimeEnd, 2.5, false);

			}
		}
	}
}

void AAI_SwordCharacter::GreenOnionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::GreenOnionBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (GEngine)
		{
			auto victim = Cast<ABaseCharacter>(OtherActor);
			if (nullptr != victim)
			{
				DamagedActorCollector.insert({ victim->c_id, victim });
			}
		}
	}
}

void AAI_SwordCharacter::GreenOnionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::GreenOnionEndOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);
}

void AAI_SwordCharacter::CarrotBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::CarrotBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (GEngine)
		{
			auto victim = Cast<ABaseCharacter>(OtherActor);
			if (nullptr != victim)
			{
				DamagedActorCollector.insert({ victim->c_id, victim });
			}
		}
	}
}

void AAI_SwordCharacter::CarrotEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

float AAI_SwordCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//데미지 입힌게 폭탄일 경우 - 대부분의 경우
	AProjectile* projectile = Cast<AProjectile>(DamageCauser);

	if (projectile != nullptr)
	{
		//UE_LOG(LogTemp, Log, TEXT("Take Damage : Not Me Hit"));
		//if (GetController()->IsPlayerController())
		{
			if (nullptr != projectile->BombOwner)
			{
				Network::GetNetwork()->send_hitmyself_packet(s_socket, projectile->BombOwner->c_id, projectile->_fType);
				//UE_LOG(LogTemp, Log, TEXT("Take Damage : NotifyHit"));
			}
		}
	}

	//데미지 입힌게 사람인 경우 - 근접무기 공격을 받았을 경우
	ABaseCharacter* DMGCauserCharacter = Cast<ABaseCharacter>(DamageCauser);
	if (nullptr != DMGCauserCharacter)
	{
		//UE_LOG(LogTemp, Warning, TEXT("sword attack"));
		int m_ftype = static_cast<int>(DamageEvent.DamageTypeClass.GetDefaultObject()->DamageFalloff);
		Network::GetNetwork()->send_hitmyself_packet(s_socket, DMGCauserCharacter->c_id, m_ftype);
		UE_LOG(LogTemp, Warning, TEXT("sword attack, Take Damage : NotifyHit %d"), m_ftype);
	}

	return Damage;
}

void AAI_SwordCharacter::GreenOnionAttackStart()
{
	SM_GreenOnion->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AAI_SwordCharacter::GreenOnionAttackEnd()
{
	for (auto& p : DamagedActorCollector)
	{
		auto victim = p.second;
		TSubclassOf<UDamageType> dmgCauser;
		dmgCauser = UDamageType::StaticClass();
		dmgCauser.GetDefaultObject()->DamageFalloff = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("GO_AttackEnd"));

		if (!this->SM_GreenOnion->bHiddenInGame)
		{
			//원래는 피해감소 옵션이지만, 사용하지 않으니 내 입맛대로 fruitType을 보내주도록 한다.
			dmgCauser.GetDefaultObject()->DamageFalloff = 7.0f;
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Daepa is not Equipped %d"), c_id);

		UGameplayStatics::ApplyDamage(victim, 1, GetInstigatorController(), this, dmgCauser);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("start :"));
		UE_LOG(LogTemp, Log, TEXT("Damage Type %d"), dmgCauser.GetDefaultObject()->DamageFalloff);
	}
	DamagedActorCollector.clear();
	SM_GreenOnion->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAI_SwordCharacter::CarrotAttackStart()
{
	SM_Carrot->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AAI_SwordCharacter::CarrotAttackEnd()
{
	for (auto& p : DamagedActorCollector)
	{
		auto victim = p.second;
		TSubclassOf<UDamageType> dmgCauser;
		dmgCauser = UDamageType::StaticClass();
		dmgCauser.GetDefaultObject()->DamageFalloff = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("CarrotAttackEnd"));
		if (!this->SM_Carrot->bHiddenInGame)
		{
			dmgCauser.GetDefaultObject()->DamageFalloff = 8.0f;
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Dangeun is not Equipped %d"), c_id);
		UGameplayStatics::ApplyDamage(victim, 1, GetInstigatorController(), this, dmgCauser);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("start :"));
		UE_LOG(LogTemp, Log, TEXT("Damage Type %d"), dmgCauser.GetDefaultObject()->DamageFalloff);
	}
	DamagedActorCollector.clear();
	SM_Carrot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


bool AAI_SwordCharacter::ConnServer()
{
	Super::ConnServer();
	s_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(GAMESERVER_PORT);

	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	int rt = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		//error_display(err_num);
		//system("pause");
		UE_LOG(LogTemp, Error, TEXT("Conn Error %d"), err_num);
		//exit(0);
		closesocket(s_socket);
		return false;
	}

	recv_expover.setId(static_cast<unsigned char>(overID));

	DWORD recv_flag = 0;
	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), recv_Aicallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
			return false;
		}
	}
	return true;
}

void AAI_SwordCharacter::recvPacket()
{
	Super::recvPacket();
	DWORD recv_flag = 0;
	ZeroMemory(&recv_expover.getWsaOver(), sizeof(recv_expover.getWsaOver()));

	recv_expover.getWsaBuf().buf = reinterpret_cast<char*>(recv_expover.getBuf() + _prev_size);
	recv_expover.getWsaBuf().len = BUFSIZE - _prev_size;

	int ret = WSARecv(s_socket, &recv_expover.getWsaBuf(), 1, NULL, &recv_flag, &recv_expover.getWsaOver(), recv_Aicallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}




