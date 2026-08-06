// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "K7InterstAct.generated.h"
class ADmgDealer;
UCLASS()
class K7_API AK7InterstAct : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AK7InterstAct();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interst")
	int32 interesting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interst")
	uint8 ObId;
	ADmgDealer* manger;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	


};
