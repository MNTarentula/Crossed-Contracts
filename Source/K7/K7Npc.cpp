// Fill out your copyright notice in the Description page of Project Settings.
#include "K7Npc.h"
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "K7ClothBase.h"
#include "K7clothCivilian.h"
#include "Kismet/GameplayStatics.h"
#include "DmgDealer.h"
// Sets default values
AK7Npc::AK7Npc()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandle->LinearStiffness = 3000.f;
	PhysicsHandle->LinearDamping = 200.f;
	PhysicsHandle->InterpolationSpeed = 50.f;
	bIsBeingDragged = false;
	TargetDragLocation = FVector::ZeroVector;
	curHP = 100; maxHp = 100; sol = 3; maxSpead = 250.f;bDead = false;
	ClothMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClothMesh"));

	ClothMeshComponent->SetupAttachment(GetMesh());
}


void AK7Npc::BeginPlay()
{
	Super::BeginPlay();
	active = FMath::FRandRange(0.5, 2.5);
	trust = FMath::FRandRange(0.25, 2.0);
	chill = FMath::FRandRange(0.5, 1.5); // big more chill smaller more nervus more panic, more panic easier scary.
	peace = FMath::FRandRange(0.5, 1.5);
	this->Tags.Add(FName("interst"));
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADmgDealer::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		manger = Cast<ADmgDealer>(FoundActors[0]);
	}
	manger->regUnObj(this);
	ClothMeshComponent->SetLeaderPoseComponent(GetMesh());
	FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f); // Spawns 100 units in front
	FRotator SpawnRotation = GetActorRotation();

	// Configure optional spawn parameters of the cloth
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // Sets the character as the owner of the current cloth
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Handles collisions safely

	// Spawn the actor and capture the pointer for later usage.(cloth)
	cloth = GetWorld()->SpawnActor<AK7ClothBase>(
		StartClothClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	wearCurC();
	
}
void AK7Npc::wearCurC() {// wear the current cloth (physicliy in game)
	if (cloth) {
		ClothMeshComponent->SetSkeletalMesh(cloth->ClothMesh);
		cloth->AttachToNpc(this);
		if (cloth->faction == "medi") {// killer put on him self cloth of medic now people think he is medic (good mehanic one time you shoot after the target if not see you come to you for help and you kill)
			medic = true;
		}
	}
}
void AK7Npc::wearOfCurC() {
	if (cloth) { cloth->RemoveFromNpc(GetActorLocation()); }
	cloth = nullptr;
	ClothMeshComponent->SetSkeletalMesh(nullptr);	
}
void AK7Npc::StartDragging(FName BoneName, FVector StartLocation)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	// Ensure capsule is completely ignored by physics so it doesn't get stuck on floors (another safe check for not explode every thing)
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//  Ensure Skeletal Mesh is actively simulating physics (Ragdoll Mode, another safe check for remove possiblite of draging npc that  not dead or uncisos)
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();

	//  Grab the specified bone using the handle
	if (PhysicsHandle)
	{
		TargetDragLocation = StartLocation;

		// This attaches the handle to your chosen bone at the current world location
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			MeshComp,
			BoneName,
			StartLocation,
			MeshComp->GetSocketRotation(BoneName)
		);

		bIsBeingDragged = true;
	}
}
void AK7Npc::UpdateDragLocation(FVector NewLocation)
{
	// Simply updates the destination coordinate. Tick handles the heavy lifting.
	TargetDragLocation = NewLocation;
}

void AK7Npc::StopDragging()
{
	if (PhysicsHandle && bIsBeingDragged)
	{
		PhysicsHandle->ReleaseComponent();
		bIsBeingDragged = false;
	}
}

void AK7Npc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsBeingDragged && PhysicsHandle)
	{
		PhysicsHandle->SetTargetLocation(TargetDragLocation);
	}
	if (curHP >= 60) {
		sol = 3;
		maxSpead = 250.f;
	}
	else if (35 < curHP && curHP < 60) {
		sol = 2; //it crauliing and be like 1/2 alive and feal bad, slow walk and all like that
		maxSpead = 125.f;
	}
	else if (0 < curHP && curHP <= 35) {
		sol = 1; //no sanse,just laying on floor no moving at all but still alive
	}
	else if (curHP <= 0 && !bDead) {
		sol = 0; // dead 
		bDead = true; // Set this immediately so the block only executes once!
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp)
		{
			// This is the magic line you had in StartDragging! 
			// This keeps him solid to the floor AND solid to your line traces!
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetSimulatePhysics(true);
			MeshComp->WakeAllRigidBodies();
		}

		if (GetCapsuleComponent())
		{
			// Disable the capsule so it doesn't block the player from walking over the body
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		UE_LOG(LogTemp, Warning, TEXT("NPC Died - Ragdoll Colliding Properly"));
	}
}
void AK7Npc::scaryAdd(float scaryA, int staate) {// staate is just state what to chosse, 0 it damge bassed on chil, trust is 1 and it bassed on trust
	switch (staate)
	{
	case 0: // Damage
		scary += scaryA / chill;
		break;

	case 1: // Suspicious person, or suspicons blood on floor,
		scary += scaryA / trust;
		break;

	}
}
void AK7Npc::getDamgetf(int dmg, const FHitResult& hit) {// damge setter for your self, get damge amount, and hit info from where to where and more
	if (hit.GetActor() != nullptr) {// check for safe not really needed (becuase if hit null how the senter get the actor??(he not))
		FString Bone = hit.BoneName.ToString();
		if (Bone == TEXT("head") || Bone == TEXT("neck_01") || Bone == TEXT("neck_02")) {//head shot
			curHP -= dmg * 2;
		}
		else if (Bone == TEXT("spine_01") || Bone == TEXT("spine_02") || Bone == TEXT("spine_03") || Bone == TEXT("spine_04") || Bone == TEXT("spine_05")) {
			curHP -= dmg;
		}
		else {
			curHP -= dmg / 2;
		}
		scaryAdd(150, 0);
		FVector ImpulseDirection = hit.ImpactNormal * -1.0f;
		if (curHP > 0 && !bDead) {
			float LaunchPower = 32.0f; 
			FVector LaunchVelocity = ImpulseDirection * LaunchPower * (dmg/4);
			LaunchCharacter(LaunchVelocity, true, true);
		}
		else {
			float ImpulsePower = 150.0f; 
			FVector TotalImpulse = ImpulseDirection * ImpulsePower * (dmg/2);
			GetMesh()->AddImpulseAtLocation(
				TotalImpulse,
				hit.ImpactPoint
			);
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("My integer is %d and my integer is %d"), curHP, sol);
}
// Called to bind functionality to input
void AK7Npc::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

