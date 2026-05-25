// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaGameplayAbility_Aim.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "Character/DeltaCharacter.h"
#include "Components/DeltaCombatComponent.h"
#include "Weapon/Weapon.h"
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

	EnterFPS(Char);
	AttachWeaponToCamera(Char);
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

	GetWorld()->GetTimerManager().ClearTimer(WeaponBlendTimer);

	ADeltaCharacter* Char = GetDeltaCharacterFromActorInfo();
	AttachWeaponToMesh(Char);
	ExitFPS();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDeltaGameplayAbility_Aim::EnterFPS(ADeltaCharacter* Char)
{
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

void UDeltaGameplayAbility_Aim::ExitFPS()
{
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
}

void UDeltaGameplayAbility_Aim::AttachWeaponToCamera(ADeltaCharacter* Char)
{
	if (!FPSCamera || !Char)
	{
		return;
	}

	AWeapon* Weapon = Char->GetEquippedWeapon();
	if (!Weapon)
	{
		return;
	}

	WeaponMesh = Weapon->WeaponMesh;
	const FTransform WorldTransform = WeaponMesh->GetComponentTransform();
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	WeaponMesh->AttachToComponent(FPSCamera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	const FTransform CameraTransform = FPSCamera->GetComponentTransform();
	WeaponBlendStartLoc = CameraTransform.InverseTransformPosition(WorldTransform.GetLocation());
	WeaponBlendStartRot = CameraTransform.InverseTransformRotation(WorldTransform.GetRotation()).Rotator();

	WeaponBlendAlpha = 0.0f;
	GetWorld()->GetTimerManager().SetTimer(WeaponBlendTimer, this, &UDeltaGameplayAbility_Aim::TickWeaponBlend, 0.016f, true);
}

void UDeltaGameplayAbility_Aim::AttachWeaponToMesh(ADeltaCharacter* Char)
{
	if (!WeaponMesh || !Char)
	{
		return;
	}

	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	WeaponMesh->AttachToComponent(Char->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocket);
	WeaponMesh = nullptr;
}

void UDeltaGameplayAbility_Aim::TickWeaponBlend()
{
	if (!WeaponMesh)
	{
		GetWorld()->GetTimerManager().ClearTimer(WeaponBlendTimer);
		return;
	}

	const ADeltaCharacter* Char = GetDeltaCharacterFromActorInfo();
	const float Time = (Char && Char->CombatComponent) ? Char->CombatComponent->ADSTime : 0.15f;
	WeaponBlendAlpha += 0.016f / Time;

	if (WeaponBlendAlpha >= 1.0f)
	{
		WeaponBlendAlpha = 1.0f;
		WeaponMesh->SetRelativeLocation(WeaponCameraOffset);
		WeaponMesh->SetRelativeRotation(WeaponCameraRotation);
		GetWorld()->GetTimerManager().ClearTimer(WeaponBlendTimer);
		return;
	}

	WeaponMesh->SetRelativeLocation(FMath::Lerp(WeaponBlendStartLoc, WeaponCameraOffset, WeaponBlendAlpha));
	WeaponMesh->SetRelativeRotation(FMath::Lerp(WeaponBlendStartRot, WeaponCameraRotation, WeaponBlendAlpha));
}
