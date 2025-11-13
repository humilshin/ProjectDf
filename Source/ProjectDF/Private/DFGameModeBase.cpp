#include "DFGameModeBase.h"

#include "Characters/DFEnemyBase.h"
#include "Characters/DFHeroCharacter.h"
#include "Controllers/DFHeroController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/DFEnemySpawner.h"

ADFGameModeBase::ADFGameModeBase()
{
	// 웨이브 시스템 통합용 생성자
	PrimaryActorTick.bCanEverTick = false;
}

// ✅ 액터 사망 처리
void ADFGameModeBase::ActorDied(AActor* DeadActor)
{
	if (DeadActor == Hero)
	{
		Hero->HandleDeath();

		if (HeroController)
		{
			HeroController->SetPlayerEnabledState(false);
		}
		
		ShowFailWidget(); // 🔹 실패 위젯 표시
	}
	else if (ADFEnemyBase* DeadEnemy = Cast<ADFEnemyBase>(DeadActor))
	{
		DeadEnemy->HandleDeath(Hero);
	}
}

// ✅ 게임 시작 시 호출
void ADFGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 웨이브 시작
	if (WaveSet && WaveSet->Waves.Num() > 0)
	{
		CurrentWaveIndex = -1;
		StartNextWave();
	}
}


// ✅ 다음 웨이브로 진행
void ADFGameModeBase::StartNextWave()
{
	CurrentWaveIndex++;

	// 모든 웨이브를 다 클리어했으면 종료
	if (!WaveSet || !WaveSet->Waves.IsValidIndex(CurrentWaveIndex))
	{
		ShowSuccessWidget();
		return;
	}
	
	OnWaveInfoChanged.Broadcast(CurrentWaveIndex + 1, WaveSet->Waves.Num());
	

	Hero = Cast<ADFHeroCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	HeroController = Cast<ADFHeroController>(UGameplayStatics::GetPlayerController(this, 0));

	if (HeroController)
	{
		HeroController->SetPlayerEnabledState(false);

		FTimerHandle PlayerEnableTimerHandle;
		FTimerDelegate PlayerEnableTimerDelegate = FTimerDelegate::CreateUObject(
			HeroController,
			&ADFHeroController::SetPlayerEnabledState,
			true
		);

		GetWorldTimerManager().SetTimer(
			PlayerEnableTimerHandle,
			PlayerEnableTimerDelegate,
			StartDelay,
			false
		);
	}
	
	const FWaveInfo& CurrentWave = WaveSet->Waves[CurrentWaveIndex];
	SetupWave(CurrentWave); // 🔥 바로 실행!
}

// ✅ 웨이브 세부 세팅 및 시작
void ADFGameModeBase::SetupWave(FWaveInfo WaveData)
{
	// 맵에 배치된 모든 EnemySpawner 가져오기
	TArray<AActor*> Spawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADFEnemySpawner::StaticClass(), Spawners);
	SpawnedBoss = nullptr;

	// 각 스포너마다 SpawnInfos에 맞게 적 생성
	for (const FWaveSpawnInfo& Info : WaveData.SpawnInfos)
	{
		for (AActor* Actor : Spawners)
		{
			if (Actor->ActorHasTag(Info.SpawnerTag))
			{
				if (ADFEnemySpawner* Spawner = Cast<ADFEnemySpawner>(Actor))
				{
					Spawner->SpawnWaveEnemies(Info.EnemyClass, Info.Count, Info.SpawnInterval, Info.SpawnDelay);
				}
			}
		}
	}

	// 🔸 웨이브 클리어 조건별 분기 처리
	if (WaveData.ClearCondition == EWaveClearCondition::SurviveTime)
	{
		// 생존형 웨이브: 일정 시간 버티면 클리어
		GetWorldTimerManager().SetTimer(
			SurviveTimerHandle,
			this,
			&ADFGameModeBase::OnSurviveTimeEnd,
			WaveData.SurviveTime,
			false
		);

		// ✅ 매초 남은 시간 브로드캐스트
		GetWorldTimerManager().SetTimer(
			WaveTimerUpdateHandle,
			this,
			&ADFGameModeBase::UpdateWaveTimerTick,
			1.0f,
			true
		);
		
	}
	else if (WaveData.ClearCondition == EWaveClearCondition::KillBoss && WaveData.BossClass)
	{
		// 보스 웨이브: 보스 소환
		FVector BossSpawnLocation(20000.0, 5230.0, 1610.0);
		FRotator BossRotation(0, 0, 0);
		FActorSpawnParameters Params;

		SpawnedBoss = GetWorld()->SpawnActor<AActor>(WaveData.BossClass, BossSpawnLocation, BossRotation, Params);
		if (SpawnedBoss)
		{
			SpawnedBoss->OnDestroyed.AddDynamic(this, &ADFGameModeBase::OnBossDied);
		}
	}
}


// ✅ 보스 사망 시 호출
void ADFGameModeBase::OnBossDied(AActor* DeadActor)
{
	UE_LOG(LogTemp, Warning, TEXT("👑 보스 사망!"));
	SpawnedBoss = nullptr;
	OnSurviveTimeEnd();
}

// ✅ 웨이브 클리어 시 (생존 성공 or 보스 처치)
void ADFGameModeBase::OnSurviveTimeEnd()
{
	StopAllSpawners();
	GetWorldTimerManager().ClearTimer(SurviveTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveTimerUpdateHandle);

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADFEnemyBase::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		if (Enemy && !Enemy->IsPendingKillPending())
		{
			Enemy->Destroy();
		}
	}
	
	// 다음 웨이브로 이동
	OnWaveCleared.Broadcast();
}

// ✅ 남은 스폰 타이머 중단 (생존 웨이브 종료 시)
void ADFGameModeBase::StopAllSpawners()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADFEnemySpawner::StaticClass(), Found);

	for (AActor* Actor : Found)
	{
		if (ADFEnemySpawner* Spawner = Cast<ADFEnemySpawner>(Actor))
		{
			Spawner->StopSpawning();
		}
	}
}

void ADFGameModeBase::UpdateWaveTimerTick()
{
	if (!GetWorld()) return;

	float Remaining = GetWorldTimerManager().GetTimerRemaining(SurviveTimerHandle);
	if (Remaining < -1.f)
	{
		GetWorldTimerManager().ClearTimer(WaveTimerUpdateHandle);
		return;
	}

	OnWaveTimeUpdated.Broadcast(Remaining);
}

void ADFGameModeBase::ShowSuccessWidget()
{
	if (SuccessWidgetClass && !ActiveEndWidget)
	{
		ActiveEndWidget = CreateWidget<UUserWidget>(GetWorld(), SuccessWidgetClass);
		if (ActiveEndWidget)
		{
			ActiveEndWidget->AddToViewport();
			APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
			if (PC)
			{
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}

void ADFGameModeBase::ShowFailWidget()
{
	if (FailWidgetClass && !ActiveEndWidget)
	{
		ActiveEndWidget = CreateWidget<UUserWidget>(GetWorld(), FailWidgetClass);
		if (ActiveEndWidget)
		{
			ActiveEndWidget->AddToViewport();
			APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
			if (PC)
			{
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}