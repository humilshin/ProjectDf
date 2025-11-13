// Fill out your copyright notice in the Description page of Project Settings.


#include "DFCharacterAnimInstance.h"
#include "Characters/DFBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDFCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ADFBaseCharacter>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UDFCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	GroundSpeed = OwningCharacter->GetVelocity().Size2D();

	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D()>0.f;
}