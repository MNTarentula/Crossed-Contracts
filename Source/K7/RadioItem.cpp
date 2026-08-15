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

float ARadioItem::getInfoById(int32 id) {
	if (acces) {
		if (id < acces->infos.Num()) {
			uint8 maxsi = 0; // return most denger and why he maybe the denger like kill be higher then talk shit to a ppls.
			for (int32 i = 0;i < acces->infos[id].InnerArray.Num();i++) {
				if (acces->infos[id].InnerArray[i] > maxsi) {
					maxsi = acces->infos[id].InnerArray[i];
				}
			}
			return maxsi;
		}
	}
	return -1.f;
}
void ARadioItem::setInfoById(int32 id, uint8 info) {
	if (acces) {
		if (id < acces->infos.Num()) {
			if (info >= 0) {
				acces->infos[id].InnerArray.Add(info);
			}
		}
	}
}


