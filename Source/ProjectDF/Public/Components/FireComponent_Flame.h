#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FireComponent_Flame.generated.h"

class ADFHeroCharacter;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTDF_API UFireComponent_Flame : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFireComponent_Flame();
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
	UPROPERTY()
	ADFHeroCharacter* OwnerCharacter;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamageInterval = 0.3f; // 데미지 주기

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamagePerTick = 20.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FlameRange = 500.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FlameAngle = 30.f; // 부채꼴 각도

	//데미지 증가율
	UPROPERTY(EditAnywhere, Category="Combat")
	float DamageIncreasePerUpgrade = 1.2f;
	// 발사 간격 감소율
	UPROPERTY(EditAnywhere, Category="Combat")
	float IntervalDecreasePerUpgrade = 0.9f;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* FlameEffect;

	UPROPERTY()
	UNiagaraComponent* FlameComp;

	FTimerHandle FlameTimerHandle;

	void ApplyFlameDamage();
};
