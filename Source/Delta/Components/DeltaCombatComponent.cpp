// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaCombatComponent.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "Camera/CameraComponent.h"

UDeltaCombatComponent::UDeltaCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDeltaCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent = Cast<UDeltaAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));

	//设置默认fov
	TArray<UCameraComponent*> Cameras;
	GetOwner()->GetComponents<UCameraComponent>(Cameras);
	for (UCameraComponent* Cam : Cameras)
	{
		if (Cam->ComponentHasTag(TEXT("FPS")))
		{
			CameraComponent = Cam;
			DefaultFOV = CameraComponent->FieldOfView;
			break;
		}
	}
}

void UDeltaCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!AbilitySystemComponent)
	{
		return;
	}

	//开镜时fov切换
	const bool bAiming = AbilitySystemComponent->HasMatchingGameplayTag(DeltaGameplayTags::Status_Aiming);
	if (CameraComponent)
	{
		CameraComponent->SetFieldOfView(bAiming ? ADSFOV : DefaultFOV);
	}

}
