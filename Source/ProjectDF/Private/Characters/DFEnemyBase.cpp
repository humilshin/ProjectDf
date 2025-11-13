// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DFEnemyBase.h"
#include "Characters/DFHeroCharacter.h"
#include "Components/PlayerCurrencyComponent.h"

// Enemy Death Handle : Base를 통해 공통 기능 구현
void ADFEnemyBase::HandleDeath(AActor* Killer)
{
	if (ADFHeroCharacter* Hero = Cast<ADFHeroCharacter>(Killer))
	{
		if (UPlayerCurrencyComponent* Currency = Hero->FindComponentByClass<UPlayerCurrencyComponent>())
		{
			Currency->AddMoney(RewardGold);
		}
	}
	
	Destroy();
}
