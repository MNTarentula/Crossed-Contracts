// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DmgDealer.generated.h"

UCLASS()
class K7_API ADmgDealer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADmgDealer();
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	uint8 IDcur;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<AActor*> regOb;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<uint8> ids;
	void regUnObj(AActor* a);
	bool chFoMe(uint8 a);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

};
