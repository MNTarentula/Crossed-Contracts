// Fill out your copyright notice in the Description page of Project Settings.


#include "K7BrainNpc.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AIController.h"
#include "K7Npc.h"
#include "Kismet/GameplayStatics.h"

void UK7BrainNpc::randomPi(const FVector& Target, UWorld* worldBlock, AAIController* aicon)
{
    if (!aicon)
        return;

    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(worldBlock);

    if (!NavSys)
        return;
    FNavLocation NavPoint;
    bool bFound = NavSys->ProjectPointToNavigation(
        Target,
        NavPoint,
        FVector(200.f, 200.f, 300.f) // Search box
    );

    if (bFound)
    {


        aicon->MoveToLocation(NavPoint.Location);
    }
}

ACharacter* UK7BrainNpc::getNearstNpDir(float MaxRange, float MaxAngleDegrees, UWorld* worldBlock, AK7Npc* th) {
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(th); // that the npc not watch at him self 

    TArray<AActor*> OverlappingActors;

    // 2. Scan in a spherical range for find object that kind of Characters
    UKismetSystemLibrary::SphereOverlapActors(
        worldBlock,
        th->GetActorLocation(),
        MaxRange,
        ObjectTypes,
        ACharacter::StaticClass(), // Filter by Character
        IgnoredActors,
        OverlappingActors
    );

    ACharacter* NearestNPC = nullptr;
    float NearestDistanceSquared = MAX_flt;

    // Get your forward vector and current location
    FVector ForwardDir = th->GetActorForwardVector();
    FVector CurrentLoc = th->GetActorLocation();

    // 3. Filter by Direction and Find Nearest character
    for (AActor* Actor : OverlappingActors)
    {
        ACharacter* NPC = Cast<ACharacter>(Actor);
        if (NPC)
        {
            FVector DirToNPC = NPC->GetActorLocation() - CurrentLoc;
            float DistanceSquared = DirToNPC.SizeSquared();

            // Skip if it's further away than our current known nearest basic optimzation
            if (DistanceSquared > NearestDistanceSquared)
            {
                continue;
            }

            // Direction calculation using Dot Product
            DirToNPC.Normalize();
            float DotProduct = FVector::DotProduct(ForwardDir, DirToNPC);

            // Convert angle to threshold: cos(Angle)
            float AngleThreshold = FMath::Cos(FMath::DegreesToRadians(MaxAngleDegrees));

            if (DotProduct >= AngleThreshold)
            {
                NearestNPC = NPC;
                NearestDistanceSquared = DistanceSquared;
            }
        }
    }

    return NearestNPC;
}