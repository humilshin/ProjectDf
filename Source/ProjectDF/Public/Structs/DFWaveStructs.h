#pragma once

#include "CoreMinimal.h"
#include "DFWaveStructs.generated.h"

// ✅ 웨이브 클리어 조건
UENUM(BlueprintType)
enum class EWaveClearCondition : uint8
{
	SurviveTime UMETA(DisplayName = "Survive Time"),
	KillBoss    UMETA(DisplayName = "Kill Boss")
};

// ✅ 스폰 정보 (각 스포너 기준)
USTRUCT(BlueprintType)
struct FWaveSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	FName SpawnerTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	int32 Count = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	float SpawnInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	float SpawnDelay = 0.5f;
};

// ✅ 개별 웨이브 정보
USTRUCT(BlueprintType)
struct FWaveInfo
{
	GENERATED_BODY()

	// 웨이브 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	int32 WaveIndex = 1;
	
	// 클리어 방식
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	EWaveClearCondition ClearCondition = EWaveClearCondition::SurviveTime;

	// 생존형 웨이브의 경우 버티는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", meta=(EditCondition="ClearCondition == EWaveClearCondition::SurviveTime"))
	float SurviveTime = 60.f;

	// 보스 웨이브의 경우 스폰할 보스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave", meta=(EditCondition="ClearCondition == EWaveClearCondition::KillBoss"))
	TSubclassOf<AActor> BossClass;

	// 이번 웨이브에서 스폰할 적 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TArray<FWaveSpawnInfo> SpawnInfos;
};
