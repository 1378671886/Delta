// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaCombatComponent.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "AbilitySystemGlobals.h"

UDeltaCombatComponent::UDeltaCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDeltaCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent = Cast<UDeltaAbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

void UDeltaCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!AbilitySystemComponent || ADSTime <= 0.0f)
	{
		return;
	}

	const bool bAiming = AbilitySystemComponent->HasMatchingGameplayTag(DeltaGameplayTags::Status_Aiming);
	const float Step = DeltaTime / ADSTime;

	if (bAiming)
	{
		AimAlpha = FMath::Min(AimAlpha + Step, 1.0f);
	}
	else
	{
		AimAlpha = FMath::Max(AimAlpha - Step, 0.0f);
	}
}
