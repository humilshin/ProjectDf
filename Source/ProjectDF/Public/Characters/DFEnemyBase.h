// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/DFBaseCharacter.h"
#include "DFEnemyBase.generated.h"

UCLASS()
class PROJECTDF_API ADFEnemyBase : public ADFBaseCharacter
{
	GENERATED_BODY()

public:
	void HandleDeath(AActor* killer);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Currency")
	int32 RewardGold = 0;
};
