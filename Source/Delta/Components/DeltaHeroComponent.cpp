// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaHeroComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "Character/DeltaCharacter.h"
#include "Input/DeltaEnhancedInputComponent.h"
#include "Input/DeltaInputConfig.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Player/DeltaPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaHeroComponent)

UDeltaHeroComponent::UDeltaHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDeltaHeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

UDeltaHeroComponent* UDeltaHeroComponent::FindHeroComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	if (UDeltaHeroComponent* HC = Actor->FindComponentByClass<UDeltaHeroComponent>())
	{
		return HC;
	}

	// Fallback: look on the instigator (e.g. PlayerState for ASC-owned abilities)
	if (const APawn* Pawn = Cast<APawn>(Actor))
	{
		if (const APlayerState* PS = Pawn->GetPlayerState())
		{
			if (UDeltaHeroComponent* HC = PS->FindComponentByClass<UDeltaHeroComponent>())
			{
				return HC;
			}
		}
	}

	return nullptr;
}

APawn* UDeltaHeroComponent::GetPawn() const
{
	return Cast<APawn>(GetOwner());
}

void UDeltaHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	APawn* Pawn = GetPawn();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC)
	{
		return;
	}

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	for (const TObjectPtr<UInputMappingContext>& IMC : DefaultInputMappings)
	{
		if (IMC)
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}

	UDeltaEnhancedInputComponent* DeltaIC = Cast<UDeltaEnhancedInputComponent>(PlayerInputComponent);
	if (!ensureMsgf(DeltaIC, TEXT("Input Component must be UDeltaEnhancedInputComponent or subclass.")))
	{
		return;
	}

	DeltaIC->AddInputMappings(InputConfig, Subsystem);

	DeltaIC->BindNativeAction(InputConfig, DeltaGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
	DeltaIC->BindNativeAction(InputConfig, DeltaGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look, false);
	DeltaIC->BindNativeAction(InputConfig, DeltaGameplayTags::InputTag_Crouch, ETriggerEvent::Started, this, &ThisClass::Input_Crouch, false);
	DeltaIC->BindNativeAction(InputConfig, DeltaGameplayTags::InputTag_HoldCrouch, ETriggerEvent::Started, this, &ThisClass::Input_HoldCrouch_Pressed, false);
	DeltaIC->BindNativeAction(InputConfig, DeltaGameplayTags::InputTag_HoldCrouch, ETriggerEvent::Completed, this, &ThisClass::Input_HoldCrouch_Released, false);
	DeltaIC->BindNativeAction(InputConfig, DeltaGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, false);

	TArray<uint32> BindHandles;
	DeltaIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
}

void UDeltaHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	if (!Controller)
	{
		return;
	}

	if (ADeltaPlayerController* DeltaPC = Cast<ADeltaPlayerController>(Controller))
	{
		DeltaPC->SetIsAutoRunning(false);
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Pawn->AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Pawn->AddMovementInput(MovementDirection, Value.Y);
	}
}

void UDeltaHeroComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn();
	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}


void UDeltaHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	// Toggle crouch — always toggles on press
	if (ADeltaCharacter* Character = Cast<ADeltaCharacter>(GetPawn()))
	{
		Character->ToggleCrouch();
	}
}

void UDeltaHeroComponent::Input_HoldCrouch_Pressed(const FInputActionValue& InputActionValue)
{
	if (ADeltaCharacter* Character = Cast<ADeltaCharacter>(GetPawn()))
	{
		Character->Crouch();
	}
}

void UDeltaHeroComponent::Input_HoldCrouch_Released(const FInputActionValue& InputActionValue)
{
	if (ADeltaCharacter* Character = Cast<ADeltaCharacter>(GetPawn()))
	{
		Character->UnCrouch();
	}
}

void UDeltaHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn())
	{
		if (ADeltaPlayerController* Controller = Cast<ADeltaPlayerController>(Pawn->GetController()))
		{
			Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}
	}
}

void UDeltaHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const ADeltaCharacter* Character = Cast<ADeltaCharacter>(GetPawn()))
	{
		if (UDeltaAbilitySystemComponent* ASC = Character->GetDeltaAbilitySystemComponent())
		{
			ASC->AbilityInputTagPressed(InputTag);
		}
	}
}

void UDeltaHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const ADeltaCharacter* Character = Cast<ADeltaCharacter>(GetPawn()))
	{
		if (UDeltaAbilitySystemComponent* ASC = Character->GetDeltaAbilitySystemComponent())
		{
			ASC->AbilityInputTagReleased(InputTag);
		}
	}
}
