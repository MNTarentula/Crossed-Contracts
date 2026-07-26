// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7CombatBase.h"
#include "K7KillerBase.generated.h"

UCLASS()
class K7_API AK7KillerBase : public AK7CombatBase
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AK7KillerBase();
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AK7ClothBase*> CurrentClothes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FK7Skill> Skills;*/

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FakeName;

	

	AK7Npc* DraggedNpc;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void drugThm(FName BoneName,AK7Npc* DraNpc);
	void getThemDo (const FHitResult& Hit);
	void changeC(const FHitResult& Hit);
	void redressN(const FHitResult& Hit);
	virtual void stopDra();
};
