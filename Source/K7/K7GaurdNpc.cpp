// Fill out your copyright notice in the Description page of Project Settings.


#include "K7GaurdNpc.h"
#include "AsultAvtomatK.h"
#include "K7BrainNpc.h"
AK7GaurdNpc::AK7GaurdNpc() {
	//constructor
}

void AK7GaurdNpc::BeginPlay() {
	Super::BeginPlay();
	//spawn ak47 to the gaurd. becasue  i test after can be any other main weapon. for now one ak47 spawn and socket to the hand.
	FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f); // Spawns 100 units in front
	FRotator SpawnRotation = GetActorRotation();


	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; 
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Handles collisions safely

	CollectWeapon(GetWorld()->SpawnActor<AAsultAvtomatK>(
		weap,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	));
	socketstuck(CurrentWeapon, nullptr);
}

void AK7GaurdNpc::shotAtTarget(AK7Npc* tar) {
	if (IsValid(tar)) {
		if (CurrentWeapon) {
			FVector Direction = tar->GetActorLocation() - GetActorLocation();
			Fire(GetActorLocation(), Direction);
		}
		else {
			SwitchWeapon(0);
			if (CurrentWeapon) {
				FVector Direction = tar->GetActorLocation() - GetActorLocation();
				Fire(GetActorLocation(), Direction);
			}
			return;

		}
	}
	else {
		tar = Cast<AK7Npc>(UK7BrainNpc::getNearstNpDir(5000.f, 180.f, GetWorld(), this));
		if (CurrentWeapon) {
			FVector Direction = tar->GetActorLocation() - GetActorLocation();
			Fire(GetActorLocation(), Direction);
		}
		else {
			SwitchWeapon(0);
			if (CurrentWeapon) {
				FVector Direction = tar->GetActorLocation() - GetActorLocation();
				Fire(GetActorLocation(), Direction);
			}
			return;

		}
	}return;
}