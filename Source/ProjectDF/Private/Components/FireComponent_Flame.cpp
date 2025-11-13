#include "Components/FireComponent_Flame.h"
#include "Characters/DFHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/World.h"


UFireComponent_Flame::UFireComponent_Flame()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFireComponent_Flame::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ADFHeroCharacter>(GetOwner());
}

void UFireComponent_Flame::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (!OwnerCharacter) return;
	
	// 🔥 나이아가라 이펙트 시작
	if (FlameEffect)
	{
		if (!FlameComp)
		{
			FlameComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				FlameEffect,
				OwnerCharacter->GetMesh(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				true
			);
		}
	}

	// 🔥 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		FlameTimerHandle,
		this,
		&UFireComponent_Flame::ApplyFlameDamage,
		DamageInterval,
		true
	);

	UE_LOG(LogTemp, Warning, TEXT("[Flame] Activated"));
}

void UFireComponent_Flame::Deactivate()
{
	Super::Deactivate();

	GetWorld()->GetTimerManager().ClearTimer(FlameTimerHandle);

	if (FlameComp)
	{
		FlameComp->Deactivate();
		FlameComp = nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Flame] Deactivated"));
}

void UFireComponent_Flame::ApplyFlameDamage()
{
	if (!OwnerCharacter) return;

	const FVector Start = OwnerCharacter->GetActorLocation();
	const FVector Forward = OwnerCharacter->GetActorForwardVector();

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(FlameOverlap), false);
	Params.AddIgnoredActor(OwnerCharacter);

	const FVector Center = Start + Forward * FlameRange * 0.5f;
	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Center,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(FlameRange),
		Params
	);

	for (auto& Result : Overlaps)
	{
		AActor* Target = Result.GetActor();
		if (!Target || Target == OwnerCharacter) continue;

		const FVector ToTarget = (Target->GetActorLocation() - Start).GetSafeNormal();
		const float Degree = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Forward, ToTarget)));
		if (Degree > FlameAngle * 0.5f) continue;

		UGameplayStatics::ApplyDamage(
			Target,
			DamagePerTick,
			OwnerCharacter->GetInstigatorController(),
			OwnerCharacter,
			UDamageType::StaticClass()
		);
	}
}

void UFireComponent_Flame::Upgrade()
{
	if (UpgradeLevel == 0)
	{
		UpgradeLevel++;
		OwnerCharacter->SetAbilityActive(EHeroAbilityType::Flame, true);
	}
	else
	{
		DamagePerTick = DamagePerTick * DamageIncreasePerUpgrade;
		DamageInterval = DamageInterval * IntervalDecreasePerUpgrade;
		UpgradeLevel++;
	}
}

