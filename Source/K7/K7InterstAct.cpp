// Fill out your copyright notice in the Description page of Project Settings.


#include "K7InterstAct.h"
#include "Kismet/GameplayStatics.h"
#include "DmgDealer.h"
// Sets default values
AK7InterstAct::AK7InterstAct()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AK7InterstAct::BeginPlay()
{
	Super::BeginPlay();
	this->Tags.Add(FName("interst"));
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADmgDealer::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		manger = Cast<ADmgDealer>(FoundActors[0]);
	}
	if (manger) {
		manger->regUnObj(this);
	}
	
}

// Called every frame

