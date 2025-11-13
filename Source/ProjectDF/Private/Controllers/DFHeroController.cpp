// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/DFHeroController.h"
#include "Characters/DFHeroCharacter.h" 
//EIP
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

ADFHeroController::ADFHeroController()
{
	
}

void ADFHeroController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void ADFHeroController::SetupInputComponent()
{
	Super::SetupInputComponent(); 
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADFHeroController::Move);
		}
	}
}

void ADFHeroController::Move(const FInputActionValue& Value)
{
	const FVector2D InputAxisVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void ADFHeroController::SetPlayerEnabledState(bool bPlayerEnabled)
{

	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	if (SubSystem && DefaultMappingContext)
	{
		if (bPlayerEnabled)
		{
			SubSystem->AddMappingContext(DefaultMappingContext, 0);
		}
		else
		{
			SubSystem->RemoveMappingContext(DefaultMappingContext);
		}
	}
}
