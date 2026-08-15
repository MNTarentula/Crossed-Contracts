// Fill out your copyright notice in the Description page of Project Settings.


#include "K7BrainNpc.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AIController.h"



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