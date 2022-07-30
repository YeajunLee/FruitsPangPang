// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class FRUITSPANGPANG_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void durianPacket(const FVector& pos);

	UFUNCTION(BlueprintCallable)
		void ApplyDamageCPP(AActor* otherActor);

	UFUNCTION(BlueprintCallable)
		void SyncBananaTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		class ABaseCharacter* BombOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		int _fType;

	int uniqueID;	//바나나만 쓰는 변수. 바나나 위치 동기화를 위해 서버가 부여해주는 ID

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		int BananaJudging;	//바나나가 현재 밟혀서 서버에서 판단중이면 서버로 패킷 더이상 보내지 않음. 바나나만 쓰임.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		class UProjectileMovementComponent* ProjectileMovementComponent;
};
