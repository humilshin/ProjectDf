#pragma once

#include "CoreMinimal.h"
#include "Characters/DFEnemyBase.h"
#include "DFEnemyBoss.generated.h"

class UCapsuleComponent;
class USceneComponent;
class AProjectile;

UCLASS()
class PROJECTDF_API ADFEnemyBoss : public ADFEnemyBase
{
	GENERATED_BODY()

public:
	ADFEnemyBoss();

protected:
	virtual void BeginPlay() override;

	/**  근접 공격 관련 **/
	UPROPERTY(VisibleAnywhere, Category="Boss|Melee")
	UCapsuleComponent* AttackRangeComp;

	UPROPERTY(EditAnywhere, Category="Boss|Melee")
	float MeleeDamage = 50.f;

	UPROPERTY(EditAnywhere, Category="Boss|Melee")
	float DamageInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category="Boss|Melee")
	float MoveSpeed = 500.f;

	/** 원거리 공격 관련 **/
	UPROPERTY(VisibleAnywhere, Category="Boss|Ranged")
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, Category="Boss|Ranged")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Boss|Ranged")
	float FireInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category="Boss|Ranged")
	float FireDuration = 5.0f;

	UPROPERTY(EditAnywhere, Category="Boss|Ranged")
	float FireRange = 5000.f;

	UPROPERTY(EditAnywhere, Category="Boss|Ranged")
	float ProjectileDamage = 100.f;

	void FaceHero();

	/* 내부 제어용 */
	UPROPERTY()
	AActor* Hero;

	FTimerHandle PhaseTimerHandle;
	FTimerHandle MoveTimerHandle;
	FTimerHandle FireTimerHandle;
	FTimerHandle DamageTimerHandle;

	bool bOverlappingHero = false;
	bool bIsFiringPhase = false;

	/* 단계별 행동 */
	void StartMeleePhase();
	void StartRangedPhase();

	/* 세부 동작 */
	void MoveToHero();
	void FireProjectile();
	void TryApplyDamage();


	/* 오버랩 이벤트 */
	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
