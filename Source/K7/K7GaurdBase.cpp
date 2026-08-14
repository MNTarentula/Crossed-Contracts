// Fill out your copyright notice in the Description page of Project Settings.


#include "K7GaurdBase.h"
#include "RadioItem.h"
AK7GaurdBase::AK7GaurdBase() {
	
}
void AK7GaurdBase::BeginPlay() {
	Super::BeginPlay();
	FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f); // Spawns 100 units in front
	FRotator SpawnRotation = GetActorRotation();

	// Configure optional spawn parameters of the cloth
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // Sets the character as the owner of the current cloth
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Handles collisions safely

	// Spawn the actor and capture the pointer for later usage.(cloth)
	radioCur = GetWorld()->SpawnActor<ARadioItem>(
		startRadio,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
}
void AK7GaurdBase::settInfoById(uint8 id,float info) {
	if (radioCur) {
		radioCur->setInfoById(id, info);
	}
}