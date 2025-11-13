#include "Components/PlayerCurrencyComponent.h"

UPlayerCurrencyComponent::UPlayerCurrencyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerCurrencyComponent::AddMoney(int32 Amount)
{
	if (Amount <= 0) return;

	CurrentMoney += Amount;
	OnMoneyChanged.Broadcast(CurrentMoney);

	UE_LOG(LogTemp, Warning, TEXT("💰 Money Added: +%d (Total: %d)"), Amount, CurrentMoney);
}

bool UPlayerCurrencyComponent::SpendMoney(int32 Amount)
{
	if (Amount <= 0 || CurrentMoney < Amount)
	{
		return false;
	}

	CurrentMoney -= Amount;
	OnMoneyChanged.Broadcast(CurrentMoney);
	return true;
}
