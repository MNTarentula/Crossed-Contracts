// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "K7ClothBase.generated.h"

class AK7Npc;

UCLASS()
class K7_API AK7ClothBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AK7ClothBase();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	FString faction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	FString ClothName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	int32 alloA;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	int32 blood;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	int32 holesBreaks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	USkeletalMesh* ClothMesh = nullptr;
	
	
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	USkeletalMeshComponent* WorldMesh = nullptr;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	UStaticMeshComponent* WorldMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interst")
	int32 interesting = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AK7Npc* own;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AttachToNpc(AK7Npc* NewOwner);

	void RemoveFromNpc(FVector location);

};
