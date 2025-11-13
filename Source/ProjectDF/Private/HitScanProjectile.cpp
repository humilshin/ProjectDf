#include "HitScanProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


AHitScanProjectile::AHitScanProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHitScanProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AHitScanProjectile::Fire(const FVector& Start, const FVector& Direction, AController* InstigatorController)
{
	if (!GetWorld()) return;

	// 🔹 트레이스 파라미터 설정
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HitScan), true);
	Params.bReturnPhysicalMaterial = false;
	Params.AddIgnoredActor(this);

	if (AActor* OwnerActor = GetOwner())
	{
		Params.AddIgnoredActor(OwnerActor);
	}

	// 🔹 트레이스 끝점 계산
	const FVector Dir = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();
	const FVector End = Start + Dir * MaxRange;

	UE_LOG(LogTemp, Warning, TEXT("[Input] Direction: %s"), *Direction.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[Normalized] Dir: %s"), *Dir.ToString());
	
	// 🔹 여러 명 관통 처리용 배열
	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByChannel(
		HitResults,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	// 🔹 중복 데미지 방지용 집합
	TSet<AActor*> DamagedActors;

	FVector BeamEndPoint = End;

	// 🔹 명중 처리
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == this || HitActor == GetOwner())
			continue;

		// 같은 액터 중복 타격 방지
		if (DamagedActors.Contains(HitActor))
			continue;

		DamagedActors.Add(HitActor);

		// 데미지 적용
		UGameplayStatics::ApplyPointDamage(
			HitActor,
			Damage,
			Direction,
			Hit,
			InstigatorController,
			this,
			UDamageType::StaticClass()
		);

		// 맞은 위치에 효과
		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactEffect,
				Hit.ImpactPoint,
				Hit.ImpactNormal.Rotation()
			);
		}
	}

	// ✅ Niagara Beam 시각화
	if (BeamEffect)
	{
		UNiagaraComponent* BeamComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BeamEffect,
			Start,
			FRotator::ZeroRotator
		);
		if (BeamComp)
		{

			BeamComp->SetVariablePosition(TEXT("User.LaserStart"), Start);
			BeamComp->SetVariablePosition(TEXT("User.LaserEnd"), BeamEndPoint);
			BeamComp->SetAutoDestroy(true);
		}
	}
}
