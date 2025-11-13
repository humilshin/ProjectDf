#include "Characters/DFEnemyBoss.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Projectile.h"
#include "TimerManager.h"

/*
	보스 기능을 구현
	원거리 공격과 근접 공격을 섞음
	Timer Handle과 Delegate Binding를 이용해 반복되는 움직임 구현
*/

ADFEnemyBoss::ADFEnemyBoss()
{
	PrimaryActorTick.bCanEverTick = false;

	// 근접 충돌 감지용 캡슐 컴포넌트
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

// 초기화 및 이벤트 바인딩
void ADFEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	Hero = UGameplayStatics::GetPlayerPawn(this, 0);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MoveSpeed;
	}

	AttackRangeComp->OnComponentBeginOverlap.AddDynamic(this, &ADFEnemyBoss::OnAttackOverlapBegin);
	AttackRangeComp->OnComponentEndOverlap.AddDynamic(this, &ADFEnemyBoss::OnAttackOverlapEnd);

	StartMeleePhase();
}

// 근접 공격 단계 시작
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

	GetWorldTimerManager().SetTimer(
		PhaseTimerHandle,
		this,
		&ADFEnemyBoss::StartRangedPhase,
		5.0f,
		false
	);
}

// 원거리 공격 단계 시작
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

	GetWorldTimerManager().SetTimer(
		PhaseTimerHandle,
		this,
		&ADFEnemyBoss::StartMeleePhase,
		FireDuration,
		false
	);
}

// AI가 플레이어 방향으로 이동
void ADFEnemyBoss::MoveToHero()
{
	if (!Hero) return;

	AController* MyController = GetController();
	if (!MyController) return;

	UAIBlueprintHelperLibrary::SimpleMoveToActor(MyController, Hero);
}

// 투사체 발사 로직
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

// 근접 공격 범위 진입 시 데미지 루프 시작
void ADFEnemyBoss::OnAttackOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bIsFiringPhase) return;
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

// 근접 공격 범위 이탈 시 데미지 루프 종료
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

// 근접 피해 적용
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

// 보스가 플레이어 방향을 바라보도록 회전 조정
void ADFEnemyBoss::FaceHero()
{
	if (!Hero) return;

	FVector ToTarget = Hero->GetActorLocation() - GetActorLocation();
	FRotator LookAtRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll = 0.f;

	SetActorRotation(LookAtRot);
}
