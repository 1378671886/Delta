// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapon/Weapon.h"
#include "Character/DeltaCharacter.h"
#include "Components/DeltaEquipmentManagerComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(WeaponMesh);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereBeginOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		OverlappingPawns.AddUnique(Pawn);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		OverlappingPawns.Remove(Pawn);
	}
}

bool AWeapon::CanBePickedUpBy(APawn* Pawn) const
{
	return Pawn && OverlappingPawns.Contains(Pawn);
}

void AWeapon::PickUp(APawn* Pawn)
{
	if (!Pawn || !CanBePickedUpBy(Pawn))
	{
		return;
	}

	if (UDeltaEquipmentManagerComponent* EquipmentMgr = Pawn->FindComponentByClass<UDeltaEquipmentManagerComponent>())
	{
		EquipmentMgr->EquipWeapon(this);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
