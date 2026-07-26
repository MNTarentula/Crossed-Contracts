#include "K7RangedWeapons.h"

AK7RangedWeapons::AK7RangedWeapons()
{
	// You can put generic ranged defaults here if you want
}

void AK7RangedWeapons::Use()
{
	WeaponData.CurrentAmmo -= 1;
	// Guns fire here
	UE_LOG(LogTemp, Warning, TEXT("Bang! Fired: %s"), *WeaponData.Name);
}
void AK7RangedWeapons::Reload()
{
	WeaponData.CurrentAmmo = WeaponData.Ammo;
}
void AK7RangedWeapons::Physic(bool s)
{
	
}