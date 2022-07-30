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

	int uniqueID;	//�ٳ����� ���� ����. �ٳ��� ��ġ ����ȭ�� ���� ������ �ο����ִ� ID

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		int BananaJudging;	//�ٳ����� ���� ������ �������� �Ǵ����̸� ������ ��Ŷ ���̻� ������ ����. �ٳ����� ����.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		class UProjectileMovementComponent* ProjectileMovementComponent;
};
