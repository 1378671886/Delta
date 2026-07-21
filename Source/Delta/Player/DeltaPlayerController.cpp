// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaPlayerController.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Player/DeltaPlayerState.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaPlayerController)

ADeltaPlayerController::ADeltaPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ADeltaPlayerController::SetIsAutoRunning(bool bEnabled)
{
	bIsAutoRunning = bEnabled;
}

bool ADeltaPlayerController::GetIsAutoRunning() const
{
	return bIsAutoRunning;
}

void ADeltaPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	ProcessAutoRun(DeltaTime);
}

void ADeltaPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	ProcessAbilityInput(DeltaTime, bGamePaused);
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ADeltaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem) && InventoryIMC)
	{
		Subsystem->AddMappingContext(InventoryIMC, 1);
	}

	InventoryComponent = FindComponentByClass<UInv_InventoryComponent>();
}

void ADeltaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ADeltaPlayerController::PrimaryInteract);
	EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ADeltaPlayerController::ToggleInventory);
}

void ADeltaPlayerController::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if (const ADeltaPlayerState* DeltaPS = GetPlayerState<ADeltaPlayerState>())
	{
		if (UDeltaAbilitySystemComponent* ASC = DeltaPS->GetDeltaAbilitySystemComponent())
		{
			ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}
}

void ADeltaPlayerController::ProcessAutoRun(float DeltaTime)
{
	if (!bIsAutoRunning)
	{
		return;
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDirection = YawRotation.RotateVector(FVector::ForwardVector);
	MyPawn->AddMovementInput(ForwardDirection, 1.0f);
}

void ADeltaPlayerController::PrimaryInteract()
{
}

void ADeltaPlayerController::ToggleInventory()
{
	if (!InventoryComponent.IsValid()) return;
	InventoryComponent->ToggleInventoryMenu();
}
