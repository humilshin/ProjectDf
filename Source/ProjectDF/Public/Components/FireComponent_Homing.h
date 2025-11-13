// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FireComponent_Homing.generated.h"

class AProjectile;
class ADFHeroCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTDF_API UFireComponent_Homing : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFireComponent_Homing();
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	
	UPROPERTY(EditDefaultsOnly, Category="Projectiles")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Level")
	int32 UpgradeLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Level")
	int32 MaxLevel = 10;
	
	
	UFUNCTION(BlueprintCallable)
	void Upgrade();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	ADFHeroCharacter* OwnerCharacter;
	
	FTimerHandle FireTimerHandle;
	void FireProjectile();
	AActor* FindNearestEnemy(const FVector& From, float MaxRange);


	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float FireInterval = 1.0f; // 1초마다 발사

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float FireDamage = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float DamageIncreasePerUpgrade = 1.2f; // 1초마다 발사

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float IntervalDecreasePerUpgrade = 0.9f;
};
