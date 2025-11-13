#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DataAssets/DFWaveDataAssetBase.h"        // ✅ 웨이브 데이터셋 (여러 웨이브를 포함)
#include "DFGameModeBase.generated.h"

class ADFEnemySpawner;
class ADFHeroCharacter;
class ADFHeroController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveInfoChanged, int32, CurrentWave, int32, TotalWaves);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveTimeUpdated, float, RemainingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveCleared);

UCLASS()
class PROJECTDF_API ADFGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADFGameModeBase();

	UPROPERTY(BlueprintAssignable, Category="Wave")
	FOnWaveInfoChanged OnWaveInfoChanged;

	UPROPERTY(BlueprintAssignable, Category="Wave")
	FOnWaveTimeUpdated OnWaveTimeUpdated;

	UPROPERTY(BlueprintAssignable, Category="Wave")
	FOnWaveCleared OnWaveCleared;
	
	void ActorDied(AActor* DeadActor);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UUserWidget> SuccessWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UUserWidget> FailWidgetClass;
	
private:

	UPROPERTY()
	ADFHeroCharacter* Hero;

	UPROPERTY()
	ADFHeroController* HeroController;

	UPROPERTY(EditAnywhere, Category="Game Start")
	float StartDelay = 1.f;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Wave System")
	UDFWaveDataAssetBase* WaveSet;

	// 다음 웨이브로 이동
	UFUNCTION(BlueprintCallable)
	void StartNextWave();

	UFUNCTION(BlueprintPure) int32 GetCurrentWave() const { return CurrentWaveIndex; }
	UFUNCTION(BlueprintPure) int32 GetTotalWaves() const { return WaveSet ? WaveSet->Waves.Num() : 0; }

private:
	// 현재 웨이브 인덱스
	int32 CurrentWaveIndex = -1;
	

	// 현재 스폰된 보스 (KillBoss 모드일 때)
	UPROPERTY()
	AActor* SpawnedBoss = nullptr;

	// 생존형 웨이브 타이머
	FTimerHandle SurviveTimerHandle;
	FTimerHandle WaveTimerUpdateHandle;
	
	void UpdateWaveTimerTick();

	// 내부 로직
	UFUNCTION()
	void SetupWave(FWaveInfo WaveData);
	UFUNCTION()
	void OnBossDied(AActor* DeadActor);
	UFUNCTION()
	void OnSurviveTimeEnd();
	UFUNCTION()
	void StopAllSpawners();

	UPROPERTY()
	UUserWidget* ActiveEndWidget;

	void ShowSuccessWidget();
	void ShowFailWidget();
};
