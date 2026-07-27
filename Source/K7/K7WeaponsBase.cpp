#include "K7WeaponsBase.h"

AK7WeaponsBase::AK7WeaponsBase()
{
	PrimaryActorTick.bCanEverTick = false; // Saves performance
	this->Tags.Add(FName("interst"));//it tag mean foucs of the npc can over from one thing to that thing.
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