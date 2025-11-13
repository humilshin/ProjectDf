#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DFEnemySpawner.generated.h"

UCLASS()
class PROJECTDF_API ADFEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	ADFEnemySpawner();

	UFUNCTION(BlueprintCallable)
	void SpawnWaveEnemies(const TSubclassOf<AActor>& EnemyClass, int32 Count, float Interval, float Delay);

	UFUNCTION(BlueprintCallable)
	void StopSpawning(); 

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle SpawnTimer;
	FTimerHandle DelayTimer;

	TSubclassOf<AActor> SpawnEnemyClass;
	
	int32 SpawnedCount = 0;
	int32 TotalCount = 0;

	float SpawnInterval;
	float SpawnDelay = 0.0f;

	void SpawnOneEnemy();
	void StartSpawning();
};
