#include "Components/HeroUIComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Characters/DFHeroCharacter.h"
#include "Components/PlayerCurrencyComponent.h"
#include "DFGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"

UHeroUIComponent::UHeroUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHeroUIComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	// --------------------------
	// 🔹 UI 위젯 생성
	// --------------------------
	if (HeroUIWidgetClass)
	{
		HeroUIWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), HeroUIWidgetClass);
		if (HeroUIWidgetInstance)
		{
			HeroUIWidgetInstance->AddToViewport();

			// 💡 BindWidget은 BP 기반에서 자동 매핑됨
			MoneyText = Cast<UTextBlock>(HeroUIWidgetInstance->GetWidgetFromName(TEXT("MoneyText")));
			HealthBar = Cast<UProgressBar>(HeroUIWidgetInstance->GetWidgetFromName(TEXT("HealthBar")));
			WaveText  = Cast<UTextBlock>(HeroUIWidgetInstance->GetWidgetFromName(TEXT("WaveText")));
			TimerText = Cast<UTextBlock>(HeroUIWidgetInstance->GetWidgetFromName(TEXT("TimerText")));
		
		}
	}

	// --------------------------
	// 🔹 컴포넌트 참조 가져오기
	// --------------------------
	CurrencyComp = Owner->FindComponentByClass<UPlayerCurrencyComponent>();
	HealthComp = Owner->FindComponentByClass<UHealthComponent>();

	// --------------------------
	// 🔹 델리게이트 바인딩
	// --------------------------
	if (CurrencyComp)
	{
		CurrencyComp->OnMoneyChanged.AddDynamic(this, &UHeroUIComponent::OnMoneyChanged);
		OnMoneyChanged(CurrencyComp->GetMoney()); // 초기값 갱신
	}

	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &UHeroUIComponent::OnHealthChanged);
		OnHealthChanged(HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth()); // 초기화
	}
	
	ADFGameModeBase* GM = Cast<ADFGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->OnWaveInfoChanged.AddDynamic(this, &UHeroUIComponent::OnWaveInfoChanged);
		GM->OnWaveTimeUpdated.AddDynamic(this, &UHeroUIComponent::OnWaveTimeUpdated);
		GM->OnWaveCleared.AddDynamic(this, &UHeroUIComponent::OpenShop);
	}

	if (WaveText)
	{
		const int32 Current = GM->GetCurrentWave() + 1;   // 화면 표시는 1부터
		const int32 Total   = GM->GetTotalWaves();
		OnWaveInfoChanged(Current, Total);
	}
	
}

void UHeroUIComponent::OnMoneyChanged(int32 NewMoney)
{
	if (MoneyText)
	{
		FText DisplayText = FText::Format(NSLOCTEXT("HeroUI", "Money", "{0}"), FText::AsNumber(NewMoney));
		MoneyText->SetText(DisplayText);
	}
}

void UHeroUIComponent::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0)
	{
		float Ratio = CurrentHealth / MaxHealth;
		HealthBar->SetPercent(Ratio);
	}
}

void UHeroUIComponent::OnWaveInfoChanged(int32 CurrentWave, int32 TotalWaves)
{
	if (WaveText)
	{
		FText Display = FText::Format(
			NSLOCTEXT("HeroUI", "Wave", "Wave {0} / {1}"),
			FText::AsNumber(CurrentWave),
			FText::AsNumber(TotalWaves)
		);
		WaveText->SetText(Display);
	}
}

void UHeroUIComponent::OnWaveTimeUpdated(float RemainingTime)
{
	if (!TimerText) return;

	if (RemainingTime <= 0.f)
	{
		TimerText->SetText(FText::GetEmpty());         
		return;
	}
	
	int32 Seconds = FMath::CeilToInt(RemainingTime);
	FText Display = FText::Format(
		NSLOCTEXT("HeroUI", "WaveTime", "{0} sec left"),
		FText::AsNumber(Seconds)
	);
	TimerText->SetText(Display);
	
}

void UHeroUIComponent::OpenShop()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetPause(true);
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());

		if (ShopWidgetClass && !ShopWidgetInstance)
		{
			ShopWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ShopWidgetClass);
			if (ShopWidgetInstance)
			{
				ShopWidgetInstance->AddToViewport();
			}
		}
	}
}

void UHeroUIComponent::CloseShop()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetPause(false);
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}

	if (ShopWidgetInstance)
	{
		ShopWidgetInstance->RemoveFromParent();
		ShopWidgetInstance = nullptr;
	}

	// ✅ 상점 닫을 때 다음 웨이브 시작
	if (ADFGameModeBase* GM = Cast<ADFGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GM->StartNextWave();
	}
}