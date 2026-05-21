// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/DeltaAbilitySet.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UAnimInstance;
class UBoxComponent;
class UInputMappingContext;

UCLASS()
class DELTA_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Components")
	UBoxComponent* CollisionBox;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Abilities")
	TObjectPtr<UDeltaAbilitySet> EquipmentAbilitySet;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Input")
	TObjectPtr<UInputMappingContext> EquipmentInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animation")
	TSubclassOf<UAnimInstance> EquipmentAnimLayer;

	FDeltaAbilitySet_GrantedHandles GrantedHandles;

	bool CanBePickedUpBy(APawn* Pawn) const;
	void PickUp(APawn* Pawn);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	TArray<TObjectPtr<APawn>> OverlappingPawns;

public:
	virtual void Tick(float DeltaTime) override;
};
