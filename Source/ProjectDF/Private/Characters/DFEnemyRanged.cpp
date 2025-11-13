#include "Characters/DFEnemyRanged.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ADFEnemyRanged::ADFEnemyRanged()
{
	PrimaryActorTick.bCanEverTick = true;

	// 투사체 스폰 포인트
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

	// 이동 속도 및 RVO 회피 설정
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = Speed;
		MoveComp->bUseRVOAvoidance = true;
		MoveComp->AvoidanceConsiderationRadius = 100.f;
		MoveComp->AvoidanceWeight = 0.5f;
	}
}

/* 
	매 프레임 주변 원거리 적과 거리 유지 이동
	-> RVO Avoidance를 이용하기 위해 작은 움직임을 부여
*/
void ADFEnemyRanged::Tick(float DeltaTime)
{
	TArray<AActor*> Overlaps;
	GetOverlappingActors(Overlaps, ADFEnemyRanged::StaticClass());

	for (AActor* Other : Overlaps)
	{
		if (Other == this) continue;

		FVector Dir = GetActorLocation() - Other->GetActorLocation();
		Dir.Z = 0;
		Dir.Normalize();

		AddMovementInput(Dir, 0.3f);
	}
}

// 초기 설정 및 공격 루프 시작
void ADFEnemyRanged::BeginPlay()
{
	Super::BeginPlay();

	Hero = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Hero) return;

	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ADFEnemyRanged::FireProjectile,
		FireInterval,
		true,
		1.0f
	);
}

// 투사체 발사
void ADFEnemyRanged::FireProjectile()
{
	if (!Hero || !ProjectileClass) return;

	FaceHero();

	const FVector MyLocation = ProjectileSpawnPoint->GetComponentLocation();
	const FVector TargetLocation = Hero->GetActorLocation();
	const FVector TempDirection = (TargetLocation - MyLocation).GetSafeNormal();
	const FVector Direction = FVector(TempDirection.X, TempDirection.Y, 0.0f);

	const float Distance = FVector::Dist(MyLocation, TargetLocation);
	if (Distance > FireRange) return;

	const FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
	const FRotator SpawnRotation = Direction.Rotation();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation
	);

	if (Projectile)
	{
		Projectile->SetOwner(this);
		Projectile->SetDamage(Damage);
	}
}

// 플레이어 바라보도록 회전
void ADFEnemyRanged::FaceHero()
{
	if (!Hero) return;

	FVector ToTarget = Hero->GetActorLocation() - GetActorLocation();
	FRotator LookAtRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll = 0.f;

	SetActorRotation(LookAtRot);
}
