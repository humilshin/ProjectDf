#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitScanProjectile.generated.h"

class UNiagaraSystem;

UCLASS()
class PROJECTDF_API AHitScanProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AHitScanProjectile();

protected:
	virtual void BeginPlay() override;

private:
	// 최대 사거리
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MaxRange = 5000.f;

	// 기본 데미지
	UPROPERTY(EditAnywhere, Category = "Combat")
	float Damage = 0.f;

	// 명중 이펙트 (옵션)
	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* ImpactEffect = nullptr;

	// ✅ Niagara Beam System
	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* BeamEffect = nullptr;
public:
	// 발사 처리 함수
	void Fire(const FVector& Start, const FVector& Direction, AController* InstigatorController);

	void SetDamage(float NewDamage) { Damage = NewDamage; }
};
