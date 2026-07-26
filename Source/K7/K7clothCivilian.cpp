// Fill out your copyright notice in the Description page of Project Settings.


#include "K7clothCivilian.h"
#include "K7Npc.h"
// Sets default values
AK7clothCivilian::AK7clothCivilian()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	faction = TEXT("civi");

	ClothName= TEXT("basic shirt and pants");

	alloA=0;

	blood=0;

	holesBreaks=0;




	WorldMesh = nullptr;
	own = nullptr;
	
}

// Called when the game starts or when spawned
void AK7clothCivilian::BeginPlay()
{
	Super::BeginPlay();
	WorldMesh=FindComponentByClass<UStaticMeshComponent>();
	
	
}

// Called every frame
void AK7clothCivilian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

