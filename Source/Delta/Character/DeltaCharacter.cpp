// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaCharacter.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaAbilitySet.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "Character/DeltaPawnData.h"
#include "Components/DeltaCharacterMovementComponent.h"
#include "Components/DeltaHeroComponent.h"
#include "Player/DeltaPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerState.h"
#include "Input/DeltaEnhancedInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaCharacter)

static FName NAME_DeltaCharacterCollisionProfile_Capsule(TEXT("DeltaPawnCapsule"));
static FName NAME_DeltaCharacterCollisionProfile_Mesh(TEXT("DeltaPawnMesh"));

ADeltaCharacter::ADeltaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDeltaCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Input component — must be UDeltaEnhancedInputComponent for HeroComponent bindings
	OverrideInputComponentClass = UDeltaEnhancedInputComponent::StaticClass();

	// Capsule
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_DeltaCharacterCollisionProfile_Capsule);

	// Mesh
	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComp->SetCollisionProfileName(NAME_DeltaCharacterCollisionProfile_Mesh);

	// Movement Component
	UDeltaCharacterMovementComponent* MoveComp = CastChecked<UDeltaCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->GravityScale = 1.0f;
	MoveComp->MaxAcceleration = 2400.0f;
	MoveComp->BrakingFrictionFactor = 1.0f;
	MoveComp->BrakingFriction = 6.0f;
	MoveComp->GroundFriction = 8.0f;
	MoveComp->BrakingDecelerationWalking = 1400.0f;
	MoveComp->bUseControllerDesiredRotation = false;
	MoveComp->bOrientRotationToMovement = false;
	MoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	MoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	MoveComp->bCanWalkOffLedgesWhenCrouching = true;
	MoveComp->SetCrouchedHalfHeight(65.0f);

	// Hero Component
	HeroComponent = CreateDefaultSubobject<UDeltaHeroComponent>(TEXT("HeroComponent"));

	// Controller rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

UDeltaAbilitySystemComponent* ADeltaCharacter::GetDeltaAbilitySystemComponent() const
{
	return Cast<UDeltaAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ADeltaCharacter::GetAbilitySystemComponent() const
{
	if (const ADeltaPlayerState* DeltaPS = GetPlayerState<ADeltaPlayerState>())
	{
		return DeltaPS->GetAbilitySystemComponent();
	}
	return nullptr;
}

void ADeltaCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ADeltaCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}

bool ADeltaCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		return ASC->HasAllMatchingGameplayTags(TagContainer);
	}
	return false;
}

bool ADeltaCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		return ASC->HasAnyMatchingGameplayTags(TagContainer);
	}
	return false;
}

void ADeltaCharacter::ToggleCrouch()
{
	const UDeltaCharacterMovementComponent* MoveComp = CastChecked<UDeltaCharacterMovementComponent>(GetCharacterMovement());

	if (IsCrouched() || MoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (MoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void ADeltaCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADeltaCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADeltaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (HeroComponent)
	{
		HeroComponent->InitializePlayerInput(PlayerInputComponent);
	}
}

void ADeltaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ADeltaPlayerState* DeltaPS = GetPlayerState<ADeltaPlayerState>())
	{
		DeltaPS->GetAbilitySystemComponent()->InitAbilityActorInfo(DeltaPS, this);
		OnAbilitySystemInitialized();
	}
}

void ADeltaCharacter::UnPossessed()
{
	OnAbilitySystemUninitialized();
	Super::UnPossessed();
}

void ADeltaCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (GetPlayerState())
	{
		OnAbilitySystemInitialized();
	}
}

void ADeltaCharacter::OnAbilitySystemInitialized()
{
	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		InitializeGameplayTags();
	}

	// Grant abilities from PawnData (configured per-character via BP)
	if (PawnData)
	{
		if (ADeltaPlayerState* DeltaPS = GetPlayerState<ADeltaPlayerState>())
		{
			DeltaPS->SetPawnData(PawnData);
		}
	}
}

void ADeltaCharacter::OnAbilitySystemUninitialized()
{
	// Clear tags when ASC goes away
	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : DeltaGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}
	}
}

void ADeltaCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	const UDeltaCharacterMovementComponent* MoveComp = CastChecked<UDeltaCharacterMovementComponent>(GetCharacterMovement());
	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(MoveComp->MovementMode, MoveComp->CustomMovementMode, true);
}

void ADeltaCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = DeltaGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = DeltaGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			ASC->SetLooseGameplayTagCount(*MovementModeTag, bTagEnabled ? 1 : 0);
		}
	}
}

void ADeltaCharacter::InitializeGameplayTags()
{
	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		// Clear lingering tags
		for (const TPair<uint8, FGameplayTag>& TagMapping : DeltaGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}
		for (const TPair<uint8, FGameplayTag>& TagMapping : DeltaGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		// Set current movement mode tag
		const UDeltaCharacterMovementComponent* MoveComp = CastChecked<UDeltaCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(MoveComp->MovementMode, MoveComp->CustomMovementMode, true);
	}
}

void ADeltaCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Crouching, 1);
	}
}

void ADeltaCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponent())
	{
		ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Crouching, 0);
	}
}

bool ADeltaCharacter::CanJumpInternal_Implementation() const
{
	// Same as ACharacter but without crouch check — allows jumping while crouched
	return JumpIsAllowedInternal();
}
