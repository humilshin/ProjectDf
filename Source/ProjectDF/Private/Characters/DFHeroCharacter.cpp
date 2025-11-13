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
	// 캡슐 및 이동 설정
	GetCapsuleComponent()->InitCapsuleSize(35.f, 72.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 터렛 및 투사체 스폰 포인트
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(GetCapsuleComponent());

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	// 무기/어빌리티 컴포넌트
	FireComp_Homing   = CreateDefaultSubobject<UFireComponent_Homing>(TEXT("FireComp_Homing"));
	FireComp_HitScan  = CreateDefaultSubobject<UFireComponent_HitScan>(TEXT("FireComp_HitScan"));
	FireComp_Flame    = CreateDefaultSubobject<UFireComponent_Flame>(TEXT("FireComp_Flame"));

	// 화폐, UI, 체력 컴포넌트
	CurrencyComponent = CreateDefaultSubobject<UPlayerCurrencyComponent>(TEXT("Currency Component"));
	HeroUIComponent   = CreateDefaultSubobject<UHeroUIComponent>(TEXT("Hero UI Component"));
	HealthComponent   = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
}

void ADFHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 초기 어빌리티 활성 상태 설정
void ADFHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetAbilityActive(EHeroAbilityType::Homing,  true);
	SetAbilityActive(EHeroAbilityType::HitScan, false);
	SetAbilityActive(EHeroAbilityType::Flame,   false);
}

// 사망 처리: 캐릭터 비활성화
void ADFHeroCharacter::HandleDeath()
{
	Super::HandleDeath();

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

// 특정 어빌리티 활성/비활성 제어
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

// 이동 속도 업그레이드
void ADFHeroCharacter::UpgradeSpeed()
{
	HeroSpeed = HeroSpeed * SpeedIncreasePerUpgrade;
	GetCharacterMovement()->MaxWalkSpeed = HeroSpeed;
	SpeedUpgradeLevel++;
}
