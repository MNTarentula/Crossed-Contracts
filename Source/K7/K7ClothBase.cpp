// Fill out your copyright notice in the Description page of Project Settings.


#include "K7ClothBase.h"
#include "K7Npc.h"
#include "DmgDealer.h"

// Sets default values
AK7ClothBase::AK7ClothBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    interesting = 2;
}

// Called when the game starts or when spawned
void AK7ClothBase::BeginPlay()
{
	Super::BeginPlay();
    
}

// Called every frame
void AK7ClothBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
}
void AK7ClothBase::AttachToNpc(AK7Npc* NewOwner)
{
    if (NewOwner == nullptr)
    {
        return;
    }

    own = NewOwner;
    this->SetActorEnableCollision(false);
    if (WorldMesh)
    {
        WorldMesh->SetVisibility(false);
    }

    // later:
    // Attach cloth skeletal mesh to NPC skeleton socket/bone
}
void AK7ClothBase::RemoveFromNpc(FVector location)
{
    own = nullptr;
    this->SetActorLocation(location - GetActorRightVector()*30.f);
    this->SetActorEnableCollision(true);
    if (WorldMesh)
    {
        WorldMesh->SetVisibility(true);
    }

    // later:
    // Detach from character skeleton
    // Return to world position
}
