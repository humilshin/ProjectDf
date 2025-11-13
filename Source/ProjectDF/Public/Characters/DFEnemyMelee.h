// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/DFEnemyBase.h"
#include "DFEnemyMelee.generated.h"

class UCapsuleComponent;

UCLASS()
class PROJECTDF_API ADFEnemyMelee : public ADFEnemyBase
{
	GENERATED_BODY()

public:
	ADFEnemyMelee();
	void HandleDeath();

protected:
	virtual void BeginPlay() override;

	/** 히어로를 향해 MoveTo를 주기적으로 갱신 */
	UFUNCTION()
	void UpdateMoveToHero();

	/** 공격 범위 내에 들어온 첫 순간 또는 루프 타이밍에 데미지 시도 */
	UFUNCTION()
	void TryApplyDamage();
	/** 공격 범위 진입 */
	UFUNCTION()
	void OnAttackOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/** 공격 범위 이탈 */
	UFUNCTION()
	void OnAttackOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	/** 공격 판정용 스피어 (히어로와 겹치면 데미지 루프 시작) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Melee")
	UCapsuleComponent* AttackRangeComp;

	/** 추적 속도 (CharacterMovement MaxWalkSpeed로 반영) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Melee|Move")
	float MoveSpeed = 300.f;

	/** 1타 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Melee|Combat")
	float Damage = 10.f;

	/** 데미지 쿨타임(초) - 히어로와 겹쳐있는 동안 이 주기로 반복 타격 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Melee|Combat")
	float DamageInterval = 1.0f;

	/** MoveTo 재지정 주기(초) - 너무 자주 갱신하지 않도록 타이머로 제어 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Melee|Move")
	float PathRefreshInterval = 0.3f;

private:
	/** 타겟 히어로 */
	UPROPERTY()
	AActor* Hero = nullptr;

	/** 현재 히어로와 겹쳐있는지 플래그 (데미지 루프 유지 조건) */
	bool bOverlappingHero = false;

	/** 데미지 루프 동작 중인지 */
	bool bDamageLoopActive = false;

	/** 데미지 루프 타이머 핸들 */
	FTimerHandle DamageLoopHandle;

	/** MoveTo 갱신 타이머 핸들 */
	FTimerHandle MoveUpdateHandle;
	
};
