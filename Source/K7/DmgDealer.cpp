// Fill out your copyright notice in the Description page of Project Settings.


#include "DmgDealer.h"
#include "K7InterstAct.h"
#include "K7Npc.h"
#include "K7Civilian.h"
// Sets default values
ADmgDealer::ADmgDealer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	IDcur = 0;
}

// Called when the game starts or when spawned
void ADmgDealer::BeginPlay()
{
	Super::BeginPlay();
}
bool ADmgDealer::chFoMe(uint8 a) {
	if (-1 < a && a < regOb.Num()) { // check if what i get safe becuase i prevent crash in manger becuase here i not gonna to polish not like in civlian!
		AK7Npc* time = Cast<AK7Npc>(regOb[a]);
		return (IsValid(time));
	}
	return false;
}
void ADmgDealer::regUnObj(AActor* a)
{
	UE_LOG(LogTemp, Warning, TEXT("Manager: %p"), this);
	UE_LOG(LogTemp, Warning, TEXT("Actor: %p"), a);
	
	if (!a) { return; }

	if (!a->ActorHasTag(FName("interst"))) { return; }
	AK7InterstAct* b = Cast<AK7InterstAct>(a);
	UE_LOG(LogTemp, Warning, TEXT("Cast: %p"), b);
	if (b) {
		b->ObId = IDcur;
	}
	else {
		AK7Npc* f = Cast<AK7Npc>(a);
		if (f) { f->ObId = IDcur; } else { return; }				
	}
	for (AActor* c : regOb) {
		AK7Civilian* d = Cast<AK7Civilian>(c);
		if (!d) { continue; }

		d->idsMem.Add(0);
	}

	ids.Add(0); // currently useless
	regOb.Add(a);
	IDcur += 1;
}


