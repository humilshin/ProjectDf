#include "Components/FireComponent_HitScan.h"
#include "HitScanProjectile.h"
#include "Characters/DFHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UFireComponent_HitScan::UFireComponent_HitScan()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFireComponent_HitScan::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ADFHeroCharacter>(GetOwner());
}

void UFireComponent_HitScan::Activate(bool bReset)
{
	Super::Activate(bReset);
	if (!OwnerCharacter) return;

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&UFireComponent_HitScan::FireLaser,
		FireInterval,
		true
	);

	UE_LOG(LogTemp, Warning, TEXT("[HitScan] Activated"));
}

void UFireComponent_HitScan::Deactivate()
{
	Super::Deactivate();
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	UE_LOG(LogTemp, Warning, TEXT("[HitScan] Deactivated"));
}

void UFireComponent_HitScan::FireLaser()
{
	if (!HitScanClass || !OwnerCharacter || !OwnerCharacter->ProjectileSpawnPoint) return;

	const FVector Start = OwnerCharacter->ProjectileSpawnPoint->GetComponentLocation();
	FVector Direction;
	FRotator Rotation;

	AActor* NearestEnemy = FindNearestEnemy(Start, 1500.f);

	if (NearestEnemy)
	{
		Direction = (NearestEnemy->GetActorLocation() - Start).GetSafeNormal();
		Rotation = Direction.Rotation();
	}
	else
	{
		Direction = OwnerCharacter->GetActorForwardVector();
		Rotation = Direction.Rotation();
	}

	AHitScanProjectile* Laser = GetWorld()->SpawnActor<AHitScanProjectile>(HitScanClass, Start, Rotation);
	if (Laser)
	{
		Laser->SetOwner(OwnerCharacter);
		Laser->SetDamage(Damage);
		Laser->Fire(Start, Direction, OwnerCharacter->GetInstigatorController());
	}
}

AActor* UFireComponent_HitScan::FindNearestEnemy(const FVector& From, float MaxRange)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Actors);

	AActor* Nearest = nullptr;
	float MinDistSq = MaxRange * MaxRange;

	for (AActor* Actor : Actors)
	{
		if (Actor == OwnerCharacter) continue;

		float DistSq = FVector::DistSquared(From, Actor->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			Nearest = Actor;
		}
	}

	return Nearest;
}

void UFireComponent_HitScan::Upgrade()
{
	if (UpgradeLevel == 0)
	{
		UpgradeLevel++;
		OwnerCharacter->SetAbilityActive(EHeroAbilityType::HitScan, true);
	}
	else
	{
		Damage = Damage * DamageIncreasePerUpgrade;
		FireInterval = FireInterval * IntervalDecreasePerUpgrade;
		UpgradeLevel++;
	}
}
