// Fill out your copyright notice in the Description page of Project Settings.


#include "FoxAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFoxAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

}

void UFoxAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsinAir = Pawn->GetMovementComponent()->IsFalling();
	}
}