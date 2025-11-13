// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DFBaseCharacter.generated.h"

UCLASS()
class PROJECTDF_API ADFBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADFBaseCharacter();

	void HandleDeath();
	
};
