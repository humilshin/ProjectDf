// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DFGameModeBase.h"
#include "Characters/DFHeroCharacter.h"
#include "Characters/DFEnemyMelee.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	OnHealthChanged.Broadcast(Health, MaxHealth);
	
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
	DFGameMode = Cast<ADFGameModeBase>(UGameplayStatics::GetGameMode(this));
	
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::DamageTaken(AActor *DamagedActor, float Damage, const UDamageType *DamageType, class AController *Instigator, AActor *DamageCauser)
{
	if (Damage <= 0.f) return;

	Health -= Damage;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);

	OnHealthChanged.Broadcast(Health, MaxHealth);
	
	if (Health <= 0.f && DFGameMode)
	{
		DFGameMode->ActorDied(DamagedActor);
		
	}

}

void UHealthComponent::Heal()
{
	Health = Health + MaxHealth * HealRate;
	Health = FMath::Clamp(Health, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(Health, MaxHealth);
}
