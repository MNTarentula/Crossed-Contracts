// Fill out your copyright notice in the Description page of Project Settings.
#include "K7Npc.h"
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "K7ClothBase.h"
#include "K7clothCivilian.h"
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

// Called when the game starts or when spawned
void AK7Npc::BeginPlay()
{
	Super::BeginPlay();
	ClothMeshComponent->SetLeaderPoseComponent(GetMesh());
	FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f); // Spawns 100 units in front
	FRotator SpawnRotation = GetActorRotation();

	// 3. Configure optional spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // Sets the character as the owner
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Handles collisions safely

	// 4. Spawn the actor and capture the pointer
	cloth = GetWorld()->SpawnActor<AK7ClothBase>(
		StartClothClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	wearCurC();
	
}
void AK7Npc::wearCurC() {
	if (cloth) {
		ClothMeshComponent->SetSkeletalMesh(cloth->ClothMesh);
		cloth->AttachToNpc(this);
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

	// 1. Ensure capsule is completely ignored by physics so it doesn't get stuck on floors
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 2. Ensure Skeletal Mesh is actively simulating physics (Ragdoll Mode)
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();

	// 3. Grab the specified bone using the handle
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
// Called every frame
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
void AK7Npc::getMovedD() {

}
void AK7Npc::getDamgetf(int dmg, const FHitResult& hit) {
	if (hit.GetActor() != nullptr) {
		FString Bone = hit.BoneName.ToString();
		if (Bone == TEXT("head") || Bone == TEXT("neck_01") || Bone == TEXT("neck_02")) {
			curHP -= dmg * 2;
		}
		else if (Bone == TEXT("spine_01") || Bone == TEXT("spine_02") || Bone == TEXT("spine_03") || Bone == TEXT("spine_04") || Bone == TEXT("spine_05")) {
			curHP -= dmg;
		}
		else {
			curHP -= dmg / 2;
		}
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

