#include "Characters/DFEnemyMelee.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "TimerManager.h"

ADFEnemyMelee::ADFEnemyMelee()
{
	PrimaryActorTick.bCanEverTick = false;

	// 근접 공격 감지 영역 설정
	AttackRangeComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackSphere"));
	AttackRangeComp->SetupAttachment(GetRootComponent());
	AttackRangeComp->SetCapsuleSize(44.f, 60.f);
	AttackRangeComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRangeComp->SetCollisionObjectType(ECC_WorldDynamic);
	AttackRangeComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackRangeComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AttackRangeComp->SetGenerateOverlapEvents(true);
}

// 초기 설정 및 타이머 시작
void ADFEnemyMelee::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MoveSpeed;
	}

	Hero = UGameplayStatics::GetPlayerPawn(this, 0);

	if (AttackRangeComp)
	{
		AttackRangeComp->OnComponentBeginOverlap.AddDynamic(this, &ADFEnemyMelee::OnAttackOverlapBegin);
		AttackRangeComp->OnComponentEndOverlap.AddDynamic(this, &ADFEnemyMelee::OnAttackOverlapEnd);
	}

	GetWorldTimerManager().SetTimer(
		MoveUpdateHandle,
		this,
		&ADFEnemyMelee::UpdateMoveToHero,
		PathRefreshInterval,
		true,
		0.2f
	);
}

// 플레이어 위치로 이동을 계속 갱신
void ADFEnemyMelee::UpdateMoveToHero()
{
	if (!Hero)
		Hero = UGameplayStatics::GetPlayerPawn(this, 0);

	AController* MyController = GetController();
	if (!MyController || !Hero) return;

	UAIBlueprintHelperLibrary::SimpleMoveToActor(MyController, Hero);
}

// 근접 타격 범위 진입 시 데미지 루프 시작
void ADFEnemyMelee::OnAttackOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor != Hero) return;

	bOverlappingHero = true;

	if (!bDamageLoopActive)
	{
		bDamageLoopActive = true;
		TryApplyDamage();

		GetWorldTimerManager().SetTimer(
			DamageLoopHandle,
			this,
			&ADFEnemyMelee::TryApplyDamage,
			DamageInterval,
			true,
			DamageInterval
		);
	}
}

// 근접 타격 범위 이탈 시 데미지 루프 종료
void ADFEnemyMelee::OnAttackOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor != Hero) return;

	bOverlappingHero = false;

	if (bDamageLoopActive)
	{
		GetWorldTimerManager().ClearTimer(DamageLoopHandle);
		bDamageLoopActive = false;
	}
}

// 실제 데미지 적용
void ADFEnemyMelee::TryApplyDamage()
{
	if (!bOverlappingHero || !Hero) return;

	UGameplayStatics::ApplyDamage(
		Hero,
		Damage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
}
