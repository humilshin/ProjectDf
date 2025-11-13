#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeroUIComponent.generated.h"

class UUserWidget;
class UProgressBar;
class UTextBlock;
class UPlayerCurrencyComponent;
class UHealthComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTDF_API UHeroUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHeroUIComponent();

protected:
	virtual void BeginPlay() override;

	// --------------------------
	// 🔹 위젯 관련 설정
	// --------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HeroUIWidgetClass;  // 💡 돈 + 체력 표시용 HUD

	UPROPERTY()
	UUserWidget* HeroUIWidgetInstance;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> ShopWidgetClass;

	UPROPERTY()
	UUserWidget* ShopWidgetInstance;

	// --------------------------
	// 🔹 내부 위젯 참조 (Blueprint에서 연결)
	// --------------------------
	UPROPERTY()
	UTextBlock* MoneyText;

	UPROPERTY()
	UProgressBar* HealthBar;

	UPROPERTY()
	UTextBlock* WaveText;

	UPROPERTY()
	UTextBlock* TimerText;

	// --------------------------
	// 🔹 참조 컴포넌트
	// --------------------------
	UPROPERTY()
	UPlayerCurrencyComponent* CurrencyComp;

	UPROPERTY()
	UHealthComponent* HealthComp;

	// --------------------------
	// 🔹 이벤트 바인딩 함수
	// --------------------------
	UFUNCTION()
	void OnMoneyChanged(int32 NewMoney);

	UFUNCTION()
	void OnHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void OnWaveInfoChanged(int32 CurrentWave, int32 TotalWaves);

	UFUNCTION()
	void OnWaveTimeUpdated(float RemainingTime);

	UFUNCTION()
	void OpenShop();

	UFUNCTION(BlueprintCallable)
	void CloseShop();
};
