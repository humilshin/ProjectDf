#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCurrencyComponent.generated.h"

// 돈 변화 델리게이트 (UI나 로그와 연결 가능)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, int32, NewMoney);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTDF_API UPlayerCurrencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCurrencyComponent();

	// 돈 증가
	void AddMoney(int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Currency")
	bool SpendMoney(int32 Amount);

	// 현재 금액 확인
	int32 GetMoney() const { return CurrentMoney; }

	// 블루프린트나 UI에서 바인딩 가능
	UPROPERTY(BlueprintAssignable)
	FOnMoneyChanged OnMoneyChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Currency")
	int32 UnlockCost = 500;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Currency")
	int32 UpgradeCost = 200;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Currency")
	int32 HealCost = 100;

private:
	UPROPERTY(VisibleAnywhere, Category="Currency")
	int32 CurrentMoney = 0;
	
};
