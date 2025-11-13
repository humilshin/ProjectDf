// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/DFBaseCharacter.h"
#include "DFHeroCharacter.generated.h"

class AProjectile;
class UFireComponent_Homing;
class UFireComponent_HitScan;
class UFireComponent_Flame;
class UPlayerCurrencyComponent;
class UHeroUIComponent;
class UHealthComponent;

UENUM(BlueprintType)
enum class EHeroAbilityType : uint8
{
	Homing  UMETA(DisplayName = "Homing Fire"),
	HitScan  UMETA(DisplayName = "HitScan Fire"),
	Flame  UMETA(DisplayName = "Flame Fire"),
};

UCLASS()
class PROJECTDF_API ADFHeroCharacter : public ADFBaseCharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;
	
	ADFHeroCharacter();

	void HandleDeath();
	
	APlayerController* GetPlayerController() const {return PlayerController; }

	void SetAbilityActive(EHeroAbilityType Type, bool bEnable) const;

	UFUNCTION(BlueprintCallable)
	void UpgradeSpeed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Level")
	int32 SpeedUpgradeLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Level")
	int32 SpeedMaxLevel = 10;
	

	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// 화폐 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UPlayerCurrencyComponent* CurrencyComponent;

	// 무기 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UFireComponent_Homing* FireComp_Homing;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UFireComponent_HitScan* FireComp_HitScan;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UFireComponent_Flame* FireComp_Flame;

	// UI 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UHeroUIComponent* HeroUIComponent;

	// 체력 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UHealthComponent* HealthComponent;

private:
	APlayerController* PlayerController;

	//Speed Upgrade 증가율
	UPROPERTY(EditDefaultsOnly, Category="Speed")
	float SpeedIncreasePerUpgrade = 1.1f; 

	UPROPERTY(EditDefaultsOnly, Category="Speed")
	float HeroSpeed = 600.f;

};
