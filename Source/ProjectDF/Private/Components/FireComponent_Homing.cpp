#include "Components/FireComponent_Homing.h"
#include "Projectile.h"
#include "TimerManager.h"
#include "Characters/DFHeroCharacter.h"
#include "Kismet/GameplayStatics.h"

/*
	유도 투사체 구현
	투사체 구현을 위해 Projectile을 이용
*/

UFireComponent_Homing::UFireComponent_Homing()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFireComponent_Homing::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ADFHeroCharacter>(GetOwner());
}

// 유도 투사체 어빌리티 활성화: 주기적 발사 타이머 시작
void UFireComponent_Homing::Activate(bool bReset)
{
	Super::Activate(bReset);
	if (!OwnerCharacter) return;

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&UFireComponent_Homing::FireProjectile,
		FireInterval,
		true
	);
}

// 유도 투사체 어빌리티 비활성화: 타이머 정지
void UFireComponent_Homing::Deactivate()
{
	Super::Deactivate();
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

// 유도 투사체 발사: 가장 가까운 적을 향해 발사, 없으면 전방으로 발사
void UFireComponent_Homing::FireProjectile()
{
	if (!ProjectileClass || !OwnerCharacter || !OwnerCharacter->ProjectileSpawnPoint) return;

	const FVector SpawnLocation = OwnerCharacter->ProjectileSpawnPoint->GetComponentLocation();
	FRotator SpawnRotation;

	AActor* NearestEnemy = FindNearestEnemy(SpawnLocation, 1500.f);
	if (NearestEnemy)
	{
		SpawnRotation = (NearestEnemy->GetActorLocation() - SpawnLocation).Rotation();
	}
	else
	{
		SpawnRotation = OwnerCharacter->GetActorForwardVector().Rotation();
	}

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	if (Projectile)
	{
		Projectile->SetOwner(OwnerCharacter);
		Projectile->SetDamage(FireDamage);
	}
}

// 지정 위치 기준 가장 가까운 적 찾기
AActor* UFireComponent_Homing::FindNearestEnemy(const FVector& From, float MaxRange)
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

// 유도 투사체 어빌리티 업그레이드: 피해량 증가 및 발사 간격 감소
void UFireComponent_Homing::Upgrade()
{
	FireDamage   = FireDamage * DamageIncreasePerUpgrade;
	FireInterval = FireInterval * IntervalDecreasePerUpgrade;
	UpgradeLevel++;
}
