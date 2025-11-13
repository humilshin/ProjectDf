#include "Actors/DFEnemySpawner.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ADFEnemySpawner::ADFEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADFEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ADFEnemySpawner::SpawnWaveEnemies(const TSubclassOf<AActor>& EnemyClass, int32 Count, float Interval, float Delay)
{
	if (!EnemyClass) return;

	SpawnEnemyClass = EnemyClass;
	TotalCount = Count;
	SpawnedCount = 0;
	SpawnInterval = Interval;
	SpawnDelay = Delay;
	// ✅ 지정된 시간 후에 실제 스폰 시작
	if (SpawnDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(DelayTimer, this, &ADFEnemySpawner::StartSpawning, SpawnDelay, false);
	}
	else
	{
		StartSpawning();
	}
}

void ADFEnemySpawner::StartSpawning()
{
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ADFEnemySpawner::SpawnOneEnemy, SpawnInterval, true);
}


void ADFEnemySpawner::SpawnOneEnemy()
{
	if (SpawnedCount >= TotalCount)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimer);
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedEnemy = World->SpawnActor<AActor>(SpawnEnemyClass, GetActorLocation(), GetActorRotation(), Params);

	// 2️⃣ 스폰 성공 시 카운트 증가
	if (SpawnedEnemy)
	{
		SpawnedCount++;

		// 🔹 모든 적 스폰 완료 시 중지
		if (SpawnedCount >= TotalCount)
		{
			StopSpawning();  // ✅ 중복 방지 + 일관성 유지
		}
	}
}

void ADFEnemySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}
