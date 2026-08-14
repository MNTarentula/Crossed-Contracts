// Fill out your copyright notice in the Description page of Project Settings.


#include "RadioItem.h"
#include "Kismet/GameplayStatics.h"
#include "DmgDealer.h"
// Sets default values
ARadioItem::ARadioItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARadioItem::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADmgDealer::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		acces = Cast<ADmgDealer>(FoundActors[0]);
	}
}

float ARadioItem::getInfoById(uint8 id) {
	if (acces) {
		if (id < acces->infos.Num()) {
			return acces->infos[id];
		}
	}
	return -1.f;
}
void ARadioItem::setInfoById(uint8 id, float info) {
	if (acces) {
		if (id < acces->infos.Num()) {
			if (info >= 0.f) {
				acces->infos[id] = info;
			}
		}
	}
}


