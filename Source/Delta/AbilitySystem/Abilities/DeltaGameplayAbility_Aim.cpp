// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaGameplayAbility_Aim.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "Character/DeltaCharacter.h"
#include "Camera/CameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaGameplayAbility_Aim)

UDeltaGameplayAbility_Aim::UDeltaGameplayAbility_Aim(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationPolicy = EDeltaAbilityActivationPolicy::WhileInputActive;
}

void UDeltaGameplayAbility_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponentFromActorInfo())
	{
		ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Aiming, 1);
	}

	ADeltaCharacter* Char = GetDeltaCharacterFromActorInfo();
	if (!Char || !Char->IsLocallyControlled())
	{
		return;
	}

	// Switch to first-person camera
	TArray<UCameraComponent*> Cameras;
	Char->GetComponents<UCameraComponent>(Cameras);

	for (UCameraComponent* Cam : Cameras)
	{
		if (Cam->ComponentHasTag(FPSCameraTag))
		{
			FPSCamera = Cam;
		}
		else
		{
			TPSCamera = Cam;
		}
	}

	if (FPSCamera)
	{
		bFPSWasActive = FPSCamera->IsActive();
		FPSCamera->Activate();
		Char->SetHeadVisibleToOwner(false);
	}

	bTPSWasActive = TPSCamera && TPSCamera->IsActive();

	if (TPSCamera)
	{
		TPSCamera->Deactivate();
	}
}

void UDeltaGameplayAbility_Aim::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActivationPolicy == EDeltaAbilityActivationPolicy::WhileInputActive)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UDeltaGameplayAbility_Aim::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponentFromActorInfo())
	{
		ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Aiming, 0);
	}

	// Restore cameras to their pre-ADS state
	if (FPSCamera && !bFPSWasActive)
	{
		FPSCamera->Deactivate();
		if (ADeltaCharacter* Char = GetDeltaCharacterFromActorInfo())
		{
			Char->SetHeadVisibleToOwner(true);
		}
	}

	if (TPSCamera && bTPSWasActive)
	{
		TPSCamera->Activate();
	}

	FPSCamera = nullptr;
	TPSCamera = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
