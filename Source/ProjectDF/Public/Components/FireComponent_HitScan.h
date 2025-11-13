#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FireComponent_HitScan.generated.h"

class AHitScanProjectile;
class ADFHeroCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTDF_API UFireComponent_HitScan : public UActorComponent
{
	GENERATED_BODY()

public:
	UFireComponent_HitScan();
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

	UFUNCTION(BlueprintCallable)
	void Upgrade();

	//UpgradeLevel을 0부터 시작해서 Unlock 필요
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Level")
	int32 UpgradeLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Level")
	int32 MaxLevel = 10;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AHitScanProjectile> HitScanClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireInterval = 5.0f;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	float Damage = 200.f;

	//데미지 증가율
	UPROPERTY(EditAnywhere, Category="Combat")
	float DamageIncreasePerUpgrade = 1.2f;
	//발사 간격 감소율
	UPROPERTY(EditAnywhere, Category="Combat")
	float IntervalDecreasePerUpgrade = 0.9f;

	UPROPERTY()
	ADFHeroCharacter* OwnerCharacter;

	FTimerHandle FireTimerHandle;

private:
	void FireLaser();
	AActor* FindNearestEnemy(const FVector& From, float MaxRange);
};
