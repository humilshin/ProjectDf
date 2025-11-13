#include "Characters/DFEnemyBoss.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Projectile.h"
#include "TimerManager.h"

ADFEnemyBoss::ADFEnemyBoss()
{
	PrimaryActorTick.bCanEverTick = false;

	// 근접 충돌 영역
	AttackRangeComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackRange"));
	AttackRangeComp->SetupAttachment(GetRootComponent());
	AttackRangeComp->SetCapsuleSize(100.f, 120.f);
	AttackRangeComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRangeComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackRangeComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 투사체 스폰 포인트
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(GetRootComponent());
}

void ADFEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	Hero = UGameplayStatics::GetPlayerPawn(this, 0);
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MoveSpeed;
	}

	// 이벤트 바인딩
	AttackRangeComp->OnComponentBeginOverlap.AddDynamic(this, &ADFEnemyBoss::OnAttackOverlapBegin);
	AttackRangeComp->OnComponentEndOverlap.AddDynamic(this, &ADFEnemyBoss::OnAttackOverlapEnd);

	// 루프 시작
	StartMeleePhase();
}

void ADFEnemyBoss::StartMeleePhase()
{
	bIsFiringPhase = false;

	GetWorldTimerManager().ClearTimer(FireTimerHandle);

	GetWorldTimerManager().SetTimer(
		MoveTimerHandle,
		this,
		&ADFEnemyBoss::MoveToHero,
		0.3f,
		true
	);

	// 5초 뒤 원거리 단계로
	GetWorldTimerManager().SetTimer(
		PhaseTimerHandle,
		this,
		&ADFEnemyBoss::StartRangedPhase,
		5.0f,
		false
	);
}

void ADFEnemyBoss::StartRangedPhase()
{
	bIsFiringPhase = true;

	GetWorldTimerManager().ClearTimer(MoveTimerHandle);
	GetCharacterMovement()->StopMovementImmediately();

	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ADFEnemyBoss::FireProjectile,
		FireInterval,
		true
	);

	// 5초 후 다시 근접 단계로
	GetWorldTimerManager().SetTimer(
		PhaseTimerHandle,
		this,
		&ADFEnemyBoss::StartMeleePhase,
		FireDuration,
		false
	);
}

void ADFEnemyBoss::MoveToHero()
{
	if (!Hero) return;
	AController* MyController = GetController();
	if (!MyController) return;

	UAIBlueprintHelperLibrary::SimpleMoveToActor(MyController, Hero);
}

void ADFEnemyBoss::FireProjectile()
{
	if (!Hero || !ProjectileClass) return;

	FaceHero();
	
	const FVector MyLocation = ProjectileSpawnPoint->GetComponentLocation();
	const FVector Dir = (Hero->GetActorLocation() - MyLocation).GetSafeNormal();
	const float Distance = FVector::Dist(MyLocation, Hero->GetActorLocation());
	if (Distance > FireRange) return;

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileClass,
		MyLocation,
		Dir.Rotation()
	);

	if (Projectile)
	{
		Projectile->SetOwner(this);
		Projectile->SetDamage(ProjectileDamage);
	}
}

void ADFEnemyBoss::OnAttackOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bIsFiringPhase) return; // 원거리 단계에서는 근접데미지 비활성화
	if (!OtherActor || OtherActor != Hero) return;

	bOverlappingHero = true;

	GetWorldTimerManager().SetTimer(
		DamageTimerHandle,
		this,
		&ADFEnemyBoss::TryApplyDamage,
		DamageInterval,
		true,
		0.0f
	);
}

void ADFEnemyBoss::OnAttackOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor != Hero) return;

	bOverlappingHero = false;
	GetWorldTimerManager().ClearTimer(DamageTimerHandle);
}

void ADFEnemyBoss::TryApplyDamage()
{
	if (!bOverlappingHero || !Hero) return;
	UGameplayStatics::ApplyDamage(
		Hero,
		MeleeDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
}

void ADFEnemyBoss::FaceHero()
{
	if (!Hero) return;

	FVector ToTarget = Hero->GetActorLocation() - GetActorLocation();
	FRotator LookAtRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll = 0.f;
	SetActorRotation(LookAtRot);
}