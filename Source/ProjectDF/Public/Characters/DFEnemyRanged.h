// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/DFEnemyBase.h"
#include "DFEnemyRanged.generated.h"

class AProjectile;

UCLASS()
class PROJECTDF_API ADFEnemyRanged : public ADFEnemyBase
{
	GENERATED_BODY()
	
public:
	ADFEnemyRanged();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;
	
protected:
	virtual void BeginPlay() override;

	void Tick(float DeltaTime) override;

	
	// 🔹 발사 함수
	void FireProjectile();

	// 🔹 Hero 추적용
	void FaceHero();

	// ✅ 공격 관련
	UPROPERTY(EditAnywhere, Category = "Ranged|Combat")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Ranged|Combat")
	float FireInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Ranged|Combat")
	float FireRange = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Ranged|Combat")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Ranged|Combat")
	float Speed = 300.f;

	FTimerHandle FireTimerHandle;

	// ✅ 타깃
	UPROPERTY()
	APawn* Hero;

};