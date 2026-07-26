// Fill out your copyright notice in the Description page of Project Settings.


#include "K7KillerBase.h"
#include "K7ClothBase.h"
// Sets default values
AK7KillerBase::AK7KillerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AK7KillerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AK7KillerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DraggedNpc) {
		DraggedNpc->UpdateDragLocation(GetMesh()->GetSocketLocation(TEXT("Weaponsocket")) + GetVelocity() * 0.05f);
	}
}

// Called every frame
void AK7KillerBase::drugThm(FName BoneName,AK7Npc* DraNpc)
{
    if (DraggedNpc){return;}
    if (DraNpc->bIsBeingDragged) { return; }
    shotPos = 0;
	DraggedNpc = DraNpc;
    drugstate = 1;
	FVector CurrentLocation = GetMesh()->GetSocketLocation(TEXT("Weaponsocket")); //draSock
	DraggedNpc->StartDragging(BoneName, CurrentLocation);

}
void AK7KillerBase::stopDra() {
	if (!DraggedNpc){return;}
    drugstate = 0;
	DraggedNpc->StopDragging();
	DraggedNpc = nullptr;
	SwitchWeapon(2000001);
}
void AK7KillerBase::getThemDo(const FHitResult& Hit) {
    //UE_LOG(LogTemp, Warning, TEXT("Try pick up 3"));
    //// Trace
    //FVector End = Start + (Direction * range);
    //FHitResult Hit;
    //FCollisionQueryParams QueryParams;
    //QueryParams.AddIgnoredActor(this);
    //FCollisionShape MySphere = FCollisionShape::MakeSphere(250.0f);
    
    UE_LOG(LogTemp, Warning, TEXT("Try pick up 4"));
    // Check if hit weapon
    if (Hit.bBlockingHit && Hit.GetActor())
    {
        AK7Npc* dragy = Cast<AK7Npc>(Hit.GetActor());
        if (dragy) {
            if (dragy->sol < 1) { // now it just < but need to be <= becuase 1 it uncesion but now we cant really do it becuase he still awak and sure stay on leg even in sol 1 will be fixed in next patch if i not forget if i forgot fuck off it not my problem it the plan from start when he in sol 1 he just not died not alive
                FName Bone = Hit.BoneName;
                if (Bone == TEXT("head") || Bone == TEXT("neck_01") || Bone == TEXT("neck_02")) {
                    drugThm(TEXT("head"), dragy);
                }
                else if (Bone == TEXT("upperarm_l") || Bone == TEXT("lowerarm_l") ||  Bone == TEXT("hand_l")) { // fuck need add all fingersssssss fuck i not want open that unreal and do compile and fuck off shit i ned to do it
                    drugThm(TEXT("hand_l"), dragy);
                }
                else if (Bone == TEXT("upperarm_r") || Bone == TEXT("lowerarm_r") || Bone == TEXT("hand_r")) {
                    drugThm(TEXT("hand_r"), dragy);
                }
                else if (Bone == TEXT("thigh_l") || Bone == TEXT(" calf_l") || Bone == TEXT("foot_l") || Bone == TEXT("ball_l")) {
                    drugThm(TEXT("foot_l"), dragy);
                }
                else if (Bone == TEXT("thigh_r") || Bone == TEXT(" calf_r") || Bone == TEXT("foot_r") || Bone == TEXT("ball_r")) {
                    drugThm(TEXT("foot_r"), dragy);
                }
                else {
                    drugThm(TEXT("hand_r"), dragy);
                }
            }
        }
    }

}
void AK7KillerBase::changeC(const FHitResult& Hit) {
    UE_LOG(LogTemp, Warning, TEXT("Try pick up 111"));
    if (Hit.bBlockingHit && Hit.GetActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("Try pick up qq"));
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"),
            *Hit.GetActor()->GetName());
        AK7ClothBase* cloths = Cast<AK7ClothBase>(Hit.GetActor());
        if (cloths) {
            UE_LOG(LogTemp, Warning, TEXT("Try pick up hit"));
            if (cloths->own) {
                if (cloths->own->sol < 1) {
                    if (cloth) { cloth->RemoveFromNpc(GetActorLocation()); }
                    cloth = cloths;
                    wearCurC();
                }
            }
            else {
                if (cloth) { cloth->RemoveFromNpc(GetActorLocation()); }
                cloth = cloths;
                wearCurC();
            }
        }
    }
}
void AK7KillerBase::redressN(const FHitResult& Hit) {
    if (Hit.bBlockingHit && Hit.GetActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"),
            *Hit.GetActor()->GetName());
        AK7Npc* cloths = Cast<AK7Npc>(Hit.GetActor());
        if (cloths) {
            UE_LOG(LogTemp, Warning, TEXT("Try pick up hit2"));
            if (cloths->sol < 1) {
                cloths->wearOfCurC();
            }
        }
    }
}