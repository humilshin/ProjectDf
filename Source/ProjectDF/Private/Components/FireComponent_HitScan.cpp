#include "Components/FireComponent_HitScan.h"
#include "HitScanProjectile.h"
#include "Characters/DFHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

/*
	레이저 구현
	HitScan방식과 나이아가라 Beam을 이용해 구현
*/
UFireComponent_HitScan::UFireComponent_HitScan()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFireComponent_HitScan::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ADFHeroCharacter>(GetOwner());
}

// 레이저 어빌리티 활성화: 주기적으로 레이저 발사
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

}

// 레이저 어빌리티 비활성화: 타이머 정지
void UFireComponent_HitScan::Deactivate()
{
	Super::Deactivate();
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

}

// 레이저 발사: 가장 가까운 적을 우선 타겟, 없으면 전방으로 발사
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

// 지정 위치 기준 가장 가까운 적 찾기
AActor* UFireComponent_HitScan::FindNearestEnemy(const FVector& From, float MaxRange)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Actors);

	AActor* Nearest = nullptr;
	float MinDistSq = MaxRange * MaxRange;

	for (AActor* Actor : Actors)
	{
		if (Actor == OwnerCharacter) continue;

		const float DistSq = FVector::DistSquared(From, Actor->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			Nearest = Actor;
		}
	}

	return Nearest;
}

// 레이저 업그레이드: 첫 업그레이드 시 활성화, 이후에는 피해량/쿨타임 강화
void UFireComponent_HitScan::Upgrade()
{
	if (UpgradeLevel == 0)
	{
		UpgradeLevel++;
		OwnerCharacter->SetAbilityActive(EHeroAbilityType::HitScan, true);
	}
	else
	{
		Damage       = Damage * DamageIncreasePerUpgrade;
		FireInterval = FireInterval * IntervalDecreasePerUpgrade;
		UpgradeLevel++;
	}
}
