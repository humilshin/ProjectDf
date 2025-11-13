#include "Actors/DFEnemySpawner.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ADFEnemySpawner::ADFEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

// 스포너 초기화
void ADFEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

// 웨이브 스폰 설정 및 시작
// EnemyClass : 스폰할 적 클래스
// Count      : 총 스폰 개수
// Interval   : 개체 간 스폰 간격
// Delay      : 최초 스폰 딜레이
void ADFEnemySpawner::SpawnWaveEnemies(const TSubclassOf<AActor>& EnemyClass, int32 Count, float Interval, float Delay)
{
	if (!EnemyClass) return;

	SpawnEnemyClass = EnemyClass;
	TotalCount      = Count;
	SpawnedCount    = 0;
	SpawnInterval   = Interval;
	SpawnDelay      = Delay;

	// 지정된 딜레이 이후 스폰 시작
	if (SpawnDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			DelayTimer,
			this,
			&ADFEnemySpawner::StartSpawning,
			SpawnDelay,
			false
		);
	}
	else
	{
		StartSpawning();
	}
}

// 반복 스폰 타이머 시작
void ADFEnemySpawner::StartSpawning()
{
	GetWorldTimerManager().SetTimer(
		SpawnTimer,
		this,
		&ADFEnemySpawner::SpawnOneEnemy,
		SpawnInterval,
		true
	);
}

// 적 한 개 스폰 후 개수 체크 → 필요하면 스폰 종료
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

	AActor* SpawnedEnemy = World->SpawnActor<AActor>(
		SpawnEnemyClass,
		GetActorLocation(),
		GetActorRotation(),
		Params
	);

	// 스폰 성공 시 카운트 증가 및 종료 조건 확인
	if (SpawnedEnemy)
	{
		SpawnedCount++;

		// 모든 적 스폰 완료 시 타이머 정지
		if (SpawnedCount >= TotalCount)
		{
			StopSpawning();
		}
	}
}

// 스폰 타이머 정지
void ADFEnemySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}
