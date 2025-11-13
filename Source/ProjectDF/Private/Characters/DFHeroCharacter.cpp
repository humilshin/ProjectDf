// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DFHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/FireComponent_Homing.h"
#include "Components/FireComponent_HitScan.h"
#include "Components/FireComponent_Flame.h"
#include "Components/PlayerCurrencyComponent.h"
#include "Components/HeroUIComponent.h"
#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADFHeroCharacter::ADFHeroCharacter()
{
	//Capsule Component (Root Component) 
	GetCapsuleComponent()->InitCapsuleSize(35.f,72.f);
	
	//Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f,500.f,0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 터렛
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(GetCapsuleComponent() );

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	// 무기 컴포넌트
	FireComp_Homing = CreateDefaultSubobject<UFireComponent_Homing>(TEXT("FireComp_Homing"));
	FireComp_HitScan = CreateDefaultSubobject<UFireComponent_HitScan>(TEXT("FireComp_HitScan"));
	FireComp_Flame = CreateDefaultSubobject<UFireComponent_Flame>(TEXT("FireComp_Flame"));

	//화페 컴포넌트
	CurrencyComponent = CreateDefaultSubobject<UPlayerCurrencyComponent>(TEXT("Currency Component"));

	//UI 컴포넌트
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("Hero UI Component"));

	//Health 컴포넌트
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
}

void ADFHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADFHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetAbilityActive(EHeroAbilityType::Homing, true);
	SetAbilityActive(EHeroAbilityType::HitScan, false);
	SetAbilityActive(EHeroAbilityType::Flame, false);
}

void ADFHeroCharacter::HandleDeath()
{
	Super::HandleDeath();
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void ADFHeroCharacter::SetAbilityActive(EHeroAbilityType Type, bool bEnable) const
{
	switch (Type)
	{
	case EHeroAbilityType::Homing:
		bEnable ? FireComp_Homing->Activate(true) : FireComp_Homing->Deactivate();
		break;
	case EHeroAbilityType::HitScan:
		bEnable ? FireComp_HitScan->Activate(true) : FireComp_HitScan->Deactivate();
		break;
	case EHeroAbilityType::Flame:
		bEnable ? FireComp_Flame->Activate(true) : FireComp_Flame->Deactivate();
		break;
	}
	
}

void ADFHeroCharacter::UpgradeSpeed()
{
	HeroSpeed = HeroSpeed * SpeedIncreasePerUpgrade;
	GetCharacterMovement()->MaxWalkSpeed = HeroSpeed;
	SpeedUpgradeLevel++;
}

