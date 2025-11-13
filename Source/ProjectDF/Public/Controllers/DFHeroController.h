// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h" //EIP
#include "DFHeroController.generated.h"

//EIP 전방 선언
class UEnhancedInputComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class PROJECTDF_API ADFHeroController : public APlayerController
{
	GENERATED_BODY()

public:
	ADFHeroController();
	void SetPlayerEnabledState(bool bPlayerEnabled);

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	void Move(const FInputActionValue& Value);
	
private:
	
#pragma region Inputs
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* MoveAction;
	
#pragma endregion


	
};

