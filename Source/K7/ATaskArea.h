// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATaskArea.generated.h"
UENUM(BlueprintType)
enum class ETaskType : uint8
{
	None,
	Toilet,
	Food,
	Work,
	Shop,
	Drink,
	Talk,
	Safe,
	healthCar
};
UCLASS()
class K7_API AATaskArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATaskArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETaskType TaskType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 200.f;

};
