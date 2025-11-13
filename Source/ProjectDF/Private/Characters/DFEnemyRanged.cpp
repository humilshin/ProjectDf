// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DFEnemyRanged.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ADFEnemyRanged::ADFEnemyRanged()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);
	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = Speed;

		//RVO 회피 구현
		MoveComp->bUseRVOAvoidance = true;
		MoveComp->AvoidanceConsiderationRadius = 100.f; // 서로 인식 거리
		MoveComp->AvoidanceWeight = 0.5f;        
	}
}

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

void ADFEnemyRanged::BeginPlay()
{
	Super::BeginPlay();

	// Hero 캐시
	Hero = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Hero) return;

	// 주기적으로 발사
	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ADFEnemyRanged::FireProjectile,
		FireInterval,
		true,
		1.0f
	);
}

void ADFEnemyRanged::FireProjectile()
{
	if (!Hero || !ProjectileClass) return;

	// Hero 방향으로 회전
	FaceHero();
	
	const FVector MyLocation = ProjectileSpawnPoint->GetComponentLocation();
	const FVector TargetLocation = Hero->GetActorLocation();
	const FVector TempDirection = (TargetLocation - MyLocation).GetSafeNormal();
	const FVector Direction = FVector(TempDirection.X, TempDirection.Y, 0.0f);

	// Hero가 너무 멀면 발사 X
	const float Distance = FVector::Dist(MyLocation, TargetLocation);
	if (Distance > FireRange)
		return;

	// 투사체 생성
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

void ADFEnemyRanged::FaceHero()
{
	if (!Hero) return;

	FVector ToTarget = Hero->GetActorLocation() - GetActorLocation();
	FRotator LookAtRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll = 0.f;
	SetActorRotation(LookAtRot);
}

