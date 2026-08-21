// Fill out your copyright notice in the Description page of Project Settings.


#include "K7BrainNpc.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AIController.h"
#include "K7Npc.h"
#include "K7Civilian.h"
#include "K7ClothBase.h"
#include "Kismet/GameplayStatics.h"
#include "K7WeaponsBase.h"

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
//*** the invastigate functions ***
void UK7BrainNpc::teamBuilding(UWorld* worldBlock, FInvestigationContext investig, TArray<uint8> idsMem, float trust) {
    if (!investig.Active) { return; }// safe check for not call it when we leave and excuter cal it by  the timers like that, just check safe.
    const int32 Count = investig.evidAct.Num();
    if (Count == 0)
    {
        //CurInvesActi = EInvestigationAction::LookAround;
        return;
    }

    const float Now = worldBlock->GetTimeSeconds();

    float SequenceScore = 0.f;
    float DangerScore = 0.f;
    float HelpScore = 0.f;
    float SuspicionScore = 0.f;
    // 1 it be dead or no sense,2 pistol on floor,3 injured or crawling, 4 suspicons man, 5 strange sound, 6 draged man,7 possible murder, 8 possible stealing of cloth, 9 the suspect is murder!
    int32 curTri = 0; // same as fir but current and not started teahory randlor intagrated
    int32 triSucs = 0;// 0-100 it how he sure he right if he be sure more then 90 changes by dec and chill, but it can over 0-100 becuase man can be sure in them theaory on 1000 and small clue that say another not change them mind hard.
    for (int32 i = investig.xue; i < Count; ++i)
    {
        AActor* Ev = investig.evidAct[i];
        if (!IsValid(Ev))
        {
            continue;
        }

        const float FoundTime = investig.timeEvidFind.IsValidIndex(i) ? investig.timeEvidFind[i] : Now;

        const FVector FoundLoc = investig.locationsFi.IsValidIndex(i) ? investig.locationsFi[i] : Ev->GetActorLocation();

        const float Age = Now - FoundTime;
        const float DistFromScene = FVector::Dist(FoundLoc, investig.SceneCenter);

        // older from found time evidence matters less
        float AgeWeight = 1.f;
        if (Age > 60.f) AgeWeight = 0.4f;
        else if (Age > 30.f) AgeWeight = 0.6f;
        else if (Age > 10.f) AgeWeight = 0.8f;

        // far away evidence matters less
        float DistWeight = 1.f;
        if (DistFromScene > 1000.f) DistWeight = 0.2f;
        else if (DistFromScene > 300.f) DistWeight = 0.5f;
        else if (DistFromScene > 100.f) DistWeight = 0.8f;

        const float EvidenceWeight = AgeWeight * DistWeight;
        const float Score = EvidenceWeight * 10.f;
        SequenceScore += Score;
        if (Ev == investig.TriggerActor)
        {
            if (AK7Npc* N = Cast<AK7Npc>(Ev))
            {
                const float Speed = N->GetVelocity().Size();

                if (N->sol <= 1)
                {
                    investig.firTri = 1;

                    HelpScore += 15.f;
                    SuspicionScore += 15.f;

                    // A moving unconscious/dead actor may mean dragging or physics movement. 
                    if (Speed > 80.f)
                    {
                        investig.firTri = 6;
                        SuspicionScore += 25.f;
                        DangerScore += 8.f;
                        triSucs += 15;
                    }
                    int32 diff = FVector::Dist(N->GetActorLocation(), investig.SceneCenter);;
                    if (diff > 9) { //another check that it moved. or dragged from a place that he was found.
                        investig.firTri = 6;
                        SuspicionScore += 25.f;
                        DangerScore += 8.f;

                    }
                    continue;
                }
                else if (N->sol < 4)
                {
                    investig.firTri = 3;

                    HelpScore += 17.f;
                    SuspicionScore += 18.f;

                    if (Speed > 250.f)
                    {
                        // Injured person moving quickly may be fleeing danger.
                        SuspicionScore += 12.f;
                        DangerScore += 6.f;
                    }
                    if (N->wirdo + idsMem[N->ObId] / 2.f >= 100.f * trust) {
                        investig.curSuspect = N;
                        investig.firTri = 4;
                        SuspicionScore += N->wirdo;
                        SuspicionScore += idsMem[N->ObId] / 2.f;
                        if (Speed > 400.f)
                        {
                            SuspicionScore += 18.f;
                            triSucs += 8;
                        }
                    }
                    continue;
                }

                investig.curSuspect = N;
                investig.firTri = 4;

                SuspicionScore += N->wirdo;
                SuspicionScore += idsMem[N->ObId] / 2.f;

                if (Speed > 400.f)
                {
                    SuspicionScore += 18.f;
                    triSucs += 8;
                }

                if (N->wirdo > 30.f)
                {
                    triSucs += 10;
                }

                continue;
            }

            if (AK7WeaponsBase* Weapon = Cast<AK7WeaponsBase>(Ev))
            {
                investig.firTri = 2;

                SuspicionScore += 60.f;
                DangerScore += 10.f;
                triSucs += 10;

                continue;
            }
            AK7ClothBase* Clo = Cast<AK7ClothBase>(Ev);
            if (Clo)
            {
                if (Clo->blood > 0)
                {
                    investig.firTri = 7;

                    SuspicionScore += 70.f;
                    DangerScore += 8.f;
                    triSucs += 20;

                    if (Clo->blood > 5)
                    {
                        SuspicionScore += 20.f;
                        triSucs += 10;
                    }

                    if (Clo->faction == FString("poli") ||
                        Clo->faction == FString("milt"))
                    {
                        SuspicionScore += 20.f;
                        DangerScore += 10.f;
                        triSucs += 10;
                    }
                }

                if (Clo->holesBreaks > 0)
                {
                    SuspicionScore += 8.f;

                    if (Clo->holesBreaks > 5)
                    {
                        SuspicionScore += 12.f;
                        triSucs += 5;
                    }

                    if (Clo->faction == FString("poli") ||
                        Clo->faction == FString("milt"))
                    {
                        SuspicionScore += 20.f * AgeWeight;
                    }
                }

                if (investig.firTri == 0)
                {
                    investig.firTri = 8;// possible stealing of cloths
                    SuspicionScore += 6.f;
                }

                continue;
            }
        }
        else
        {
            if (AK7WeaponsBase* Weapon = Cast<AK7WeaponsBase>(Ev))
            {
                SuspicionScore += 18.f * EvidenceWeight;
                DangerScore += 5.f * EvidenceWeight;

                if (DistFromScene < 150.f)// near the scene sreach give more detials
                {
                    SuspicionScore += 15.f * AgeWeight;
                }

                if (investig.firTri == 1 || investig.firTri == 3 || investig.firTri == 6 || investig.firTri == 7)
                {
                    curTri = 7;

                    SuspicionScore += 100.f * AgeWeight * (DistWeight * 0.5f);
                    triSucs += FMath::RoundToInt(20.f * EvidenceWeight);

                    // Body + weapon + known suspect is a strong connected theory.
                    if (investig.curSuspect)
                    {
                        curTri = 9;
                        SuspicionScore += 35.f * EvidenceWeight;
                        triSucs += 50;
                    }

                    // Weapon very far from the victim weakens the direct connection.
                    if (DistFromScene > 700.f)
                    {
                        triSucs -= 15;
                    }
                }
                else if (investig.firTri == 4)
                {
                    curTri = 9;

                    SuspicionScore += 160.f * AgeWeight * (DistWeight * 0.25f);
                    triSucs += FMath::RoundToInt(35.f * EvidenceWeight);

                    if (DistFromScene < 150.f)
                    {
                        triSucs += 20;
                    }
                    if (DistFromScene > 700.f)
                    {
                        triSucs -= 45 * AgeWeight;
                    }
                }
                else if (investig.firTri == 8)
                {
                    // Abandoned clothing followed by a weapon can mean disguise after violence.
                    curTri = 7;

                    SuspicionScore += 45.f * EvidenceWeight;
                    triSucs += 20;
                }

                continue;
            }
            else if (AK7ClothBase* Clo = Cast<AK7ClothBase>(Ev))
            {
                const bool HasBlood = Clo->blood > 0;// for not check that evry time.
                const bool IsDamaged = Clo->holesBreaks > 0;
                const bool IsAuthorityCloth = Clo->faction == FString("poli") || Clo->faction == FString("milt");

                if (investig.firTri == 7 || investig.firTri == 1 || investig.firTri == 3 || investig.firTri == 6)
                {
                    if (HasBlood)
                    {
                        curTri = 7;

                        SuspicionScore += 35.f * AgeWeight * (DistWeight * 0.5f);
                        triSucs += FMath::RoundToInt(25.f * EvidenceWeight);

                        if (Clo->blood > 50)
                        {
                            SuspicionScore += 20.f * EvidenceWeight;
                            triSucs += 15;
                        }

                        if (DistFromScene < 150.f)
                        {
                            triSucs += 15 * AgeWeight;
                        }
                        else if (DistFromScene > 700.f)
                        {
                            triSucs -= 20 * AgeWeight;
                        }
                    }

                    if (IsDamaged)
                    {
                        SuspicionScore += 12.f * EvidenceWeight;
                        triSucs += 5;
                    }
                }

                if (investig.firTri == 4)
                {
                    if (HasBlood)
                    {
                        curTri = 9;

                        SuspicionScore += 160.f * AgeWeight * (DistWeight * 0.25f);
                        triSucs += FMath::RoundToInt(45.f * EvidenceWeight);

                        if (investig.curSuspect)
                        {
                            SuspicionScore += investig.curSuspect->wirdo * 0.5f;
                        }
                    }
                    else if (!IsDamaged)
                    {
                        // Normal clothing does not support the murder theory.
                        triSucs -= 4;
                    }
                }

                if (investig.firTri == 8)
                {
                    // More abandoned clothing supports theft/disguise theory.
                    curTri = 8;
                    SuspicionScore += 15.f * EvidenceWeight;
                    triSucs += 12;

                    if (HasBlood)
                    {
                        // Clothing theft may now connect to violence.
                        curTri = 7;
                        SuspicionScore += 40.f * EvidenceWeight;
                        triSucs += 20;
                    }
                }

                if (HasBlood && IsDamaged)
                {
                    SuspicionScore += 18.f * EvidenceWeight;
                    DangerScore += 8.f * EvidenceWeight;
                    triSucs += 10;
                }

                if (IsAuthorityCloth)
                {
                    if (Clo->faction == FString("poli"))
                    {
                        HelpScore += 6.f * AgeWeight;
                    }
                    else
                    {
                        DangerScore += 4.f * AgeWeight;
                    }

                    // Abandoned bloody police/military clothes may mean impersonation.
                    if (HasBlood || IsDamaged)
                    {
                        curTri = 8;
                        SuspicionScore += 25.f * EvidenceWeight;
                        DangerScore += 4.f * AgeWeight;
                        triSucs += 15;
                    }
                }
                else
                {
                    SuspicionScore += 8.f * EvidenceWeight;
                }

                continue;
            }
            else if (AK7Npc* N = Cast<AK7Npc>(Ev))
            {
                const float Speed = N->GetVelocity().Size();
                const float DistToTrigger =
                    IsValid(investig.TriggerActor)
                    ? FVector::Dist(N->GetActorLocation(),
                        investig.TriggerActor->GetActorLocation())
                    : DistFromScene;

                const bool IsNearTrigger = DistToTrigger < 300.f;// one big hole, if he was near but only minute after the thing is wierd maye he right now come.
                const bool IsVeryNearTrigger = DistToTrigger < 120.f;
                const bool IsRunning = Speed > 400.f;
                const bool IsMovingFast = Speed > 250.f;

                if (investig.firTri == 7 || investig.firTri == 1 || investig.firTri == 3 || investig.firTri == 6)
                {
                    if (N->sol > 1)
                    {
                        float SuspectValue = N->wirdo;

                        if (IsNearTrigger)
                        {
                            SuspectValue += 20.f;
                        }

                        if (IsVeryNearTrigger)
                        {
                            SuspectValue += 15.f;
                        }

                        if (IsRunning)
                        {
                            SuspectValue += 25.f;
                        }
                        else if (IsMovingFast)
                        {
                            SuspectValue += 10.f;
                        }

                        SuspectValue += idsMem[N->ObId] / 2.f;

                        // Replace current suspect only when this person looks more suspicious. it system for not stuck on guy that you see near body in 1 second and after second see another guy witk ak an more
                        if (!investig.curSuspect)
                        {
                            investig.curSuspect = N;
                            triSucs += 10;
                        }
                        else
                        {
                            float CurrentSuspectValue = investig.curSuspect->wirdo + idsMem[investig.curSuspect->ObId] / 2.f;

                            if (SuspectValue > CurrentSuspectValue)
                            {
                                investig.curSuspect = N;
                                triSucs += 8;
                            }
                            else if (investig.curSuspect == N)
                            {
                                // Same person appears again around connected evidence.
                                triSucs += 12;
                            }
                        }
                        if (!investig.curSuspect) { curTri = 7; }
                        SuspicionScore += SuspectValue * AgeWeight * DistWeight;

                        if (IsRunning && IsNearTrigger && AgeWeight > 0.2)
                        {
                            curTri = 9;
                            SuspicionScore += 45.f * EvidenceWeight;
                            DangerScore += 12.f;
                            triSucs += 25;
                        }
                        if (curTri == 0) {
                            curTri = investig.firTri;
                            SuspicionScore -= triSucs / 10.f;
                            triSucs -= 5.f;
                        }
                        continue;
                    }

                    if (N->sol <= 1)
                    {
                        HelpScore += 15.f * EvidenceWeight;
                        SuspicionScore += 30.f * EvidenceWeight;

                        if (curTri == 7) {
                            triSucs += 35;
                        }
                        else if (investig.curSuspect) {
                            if (curTri == 9) {
                                triSucs += 35;
                            }
                            else {
                                curTri = 9;
                                triSucs += 25;
                            }
                        }
                        else {
                            curTri = 7;
                            triSucs += 25;
                        }


                        // Multiple victims strongly support an attack/murder theory.
                        if (investig.firTri == 1 || investig.firTri == 3)
                        {
                            SuspicionScore += 35.f * EvidenceWeight;
                            DangerScore += 15.f * EvidenceWeight;
                            triSucs += 30;
                        }

                        if (DistToTrigger < 250.f)
                        {
                            triSucs += 15;
                        }
                        else if (DistToTrigger > 800.f)
                        {
                            // Could be a separate incident.
                            triSucs -= 10;
                        }
                        continue;
                        //if (firTri == 2 || curTri == 2) { //need check maybe suicide now i cant add that becuase it break the past rule if first what we find was pistol.
                        //
                        //}

                    }
                }

                if (investig.firTri == 4)
                {
                    if (investig.curSuspect == N)
                    {
                        SuspicionScore += 12.f * EvidenceWeight;
                        triSucs += 8;

                        if (IsRunning)
                        {
                            curTri = 9;
                            SuspicionScore += 30.f * EvidenceWeight;
                            triSucs += 15;
                        }
                    }
                    else if (IsValid(investig.curSuspect) && N->wirdo > investig.curSuspect->wirdo && IsNearTrigger)
                    {
                        // another person may be a more sus suspect.
                        investig.curSuspect = N;
                        SuspicionScore += 20.f * EvidenceWeight;
                        triSucs -= 5;
                    }
                }

                if (investig.firTri == 8)
                {
                    // Person Near abandoned clothes may own them, steal them, or change disguise.
                    if (IsNearTrigger)
                    {
                        investig.curSuspect = N;
                        curTri = 8;

                        SuspicionScore += 20.f * EvidenceWeight;
                        triSucs += 15;

                        if (N->wirdo > 25.f || IsRunning)
                        {
                            SuspicionScore += 25.f * EvidenceWeight;
                            triSucs += 25;
                        }
                    }
                }

                if (N->sol <= 1)
                {
                    HelpScore += 15.f * AgeWeight;
                    DangerScore += 6.f * AgeWeight;
                    SuspicionScore += 15.f * AgeWeight;
                }
                else if (N->sol < 4) // Injured, need help.
                {
                    HelpScore += 25.f * AgeWeight;
                    DangerScore += 6.f * AgeWeight;
                    SuspicionScore += 15.f * AgeWeight;
                }

                SuspicionScore += N->wirdo * AgeWeight;

                if (IsRunning)
                {
                    SuspicionScore += 12.f * AgeWeight;

                    if (IsNearTrigger)
                    {
                        SuspicionScore += 15.f * EvidenceWeight;
                    }
                }

                // calm healthy person near an injured person may be helping, not attacking.
                if (N->sol == 4 &&
                    Speed < 100.f &&
                    N->wirdo < 10.f &&
                    investig.firTri == 3)
                {
                    HelpScore += 5.f * EvidenceWeight;
                    SuspicionScore -= 4.f * EvidenceWeight;
                    triSucs -= 3;
                }

                continue;
            }
        }

    }
    investig.xue = Count;
    updTheo(triSucs, curTri, investig);
    investig.Suspicion += SuspicionScore;
    investig.MedicalConcern += HelpScore;
    investig.Danger += DangerScore;
}
void UK7BrainNpc::updTheo(int32 sucs, int32 tri,FInvestigationContext investig) {

    if (tri == 0 || sucs == 0)
    {
        return;
    }

    if (investig.curTri == 0)
    {
        investig.curTri = tri;
        investig.triSucs = sucs;
        return;
    }

    if (investig.curTri == tri)
    {
        investig.triSucs += sucs;
    }
    else if (sucs > investig.triSucs)
    {
        investig.curTri = tri;
        investig.triSucs = sucs;
    }
    else
    {
        investig.triSucs -= sucs / 3;
    }

}