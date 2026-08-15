// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DmgDealer.generated.h"
struct FInnerListWrapper
{
	TArray<uint8> InnerArray;
};
UCLASS()
class K7_API ADmgDealer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADmgDealer();
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 IDcur;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<AActor*> regOb;
	FInnerListWrapper ashagdsukafd;
	TArray<FInnerListWrapper> infos;
	void regUnObj(AActor* a);
	bool chFoMe(uint8 a);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

};
