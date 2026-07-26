#include "K7WeaponsBase.h"

AK7WeaponsBase::AK7WeaponsBase()
{
	PrimaryActorTick.bCanEverTick = false; // Saves performance
}

void AK7WeaponsBase::Use()
{
	// Default behavior if a child doesn't override it
	UE_LOG(LogTemp, Warning, TEXT("Base Weapon Used"));
}

AK7WeaponsBase* AK7WeaponsBase::taked()
{
	// Default behavior if a child doesn't override it
	UE_LOG(LogTemp, Warning, TEXT("Base Weapon Used"));
	return this;
}