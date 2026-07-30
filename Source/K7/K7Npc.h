// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "K7Npc.generated.h"

class UPhysicsHandleComponent;
class AK7ClothBase;
class USkeletalMeshComponent;
class ADmgDealer;

UCLASS()
class K7_API AK7Npc : public ACharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AK7Npc();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RealName;
	int curHP, maxHp, sol; float maxSpead;bool bDead;
	//hunger and other s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior") // it can be 100 work like many it mean he need it hard
	float hung;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float thirt;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float toilet;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float work;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float shop;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float social;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float enrgy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float safe;
	//itPriority
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior") // it can be 100 work like many it mean he need it hard
	int hungP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int thirtP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int toiletP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int workP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int shopP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int socialP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int enrgyP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int safeP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior") // it can be 100 work like many it mean he need it hard
	int hungPC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int thirtPC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int toiletPC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int workPC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int shopPC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int socialPC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int enrgyPC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int safePC;

	//it debuff not really needing but can also be like neading but unlike it come if enrgy big it mean he want drink and he not had work and if he drunk more drunk.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	int drunk;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 wirdo;// how suspicons that npc look for other people base value the other people think hmh maybe he not so bad,
	// chill it how much fast he can start panic, peace is how agro he can be fast near 0 it mean he peace full,trust it  how he can trust near zero it mean he trust fast,interst it how he can be intersting in thing if he hear gunshoot he go invistage if not nervous, active it just how active active look around many cant foucs and run to him not need many he look like giper active player (created for defend player from be get them fast)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float chill;// how he chill react if he got shoted if anything happend 0.5-1.5
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float peace;// how agro or how peacfully he can react 0.5-1.5 (if 0.5 he agro easliy it mean had chance he not run and go to attack you or anyone else)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float trust;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float dec;//detective, traits it how much he love to invastigate and it acutally change how he do it. 0.5-1.5

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float active;// how speedy he is (how much think per time, how much look around, what speed he moving like he need get fast to safe place or go to sleep after hard work day like that
	//it current like mood not bahivior if too much scary run it not be small it can be from 0-100 or like that
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float scary;// how much scary now
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	float interst;// how he interst had basiclly to do invstigate

	// i got a idea, so i will have global list arry of how much ppl you know or even better in npc list of npc (i think about hoiw it be laggy if we have 100 npc pointers at evry 100 npc like 10000 pointer maybe be laggy so i dont know here.) so idea is basic in each list we know index 1 it ben and index 0 it ivan if you know ivan trust to him grow if you talk or had previus backstory so when you know he near you and company of friends go the scary of them grow much slow becuase togther not so scary idk, and if npc go with ivan and ivan disappear scary grow faster so now what now we think about how we do that smart system hell naw now we think how i do it more basic that it work lets me cock.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "bahvior")
	AK7Npc* currekNpc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 drugstate;// 0 no drag,1 hitman drag,2 civilian drag,3 medic drag,4 combat drag. later add a possiblite of no suspicion drag and skill tree for unlock them.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AK7ClothBase* cloth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth")
	TSubclassOf<AK7ClothBase> StartClothClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cloth")
	USkeletalMeshComponent* ClothMeshComponent;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Stealth")
	FRotator RotatorHead;

	// Adjust these to limit how far the head can turn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Stealth")
	float MaxLookAngle = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Stealth")
	float HeadInterpSpeed = 5.0f;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void getDamgetf(int dmg, const FHitResult& hit);
	virtual void wearCurC();
	virtual void wearOfCurC();
	virtual void scaryAdd(float scaryA,int staate);
	UFUNCTION(BlueprintCallable, Category = "Movement|Physics")
	void StartDragging(FName BoneName, FVector StartLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interst")
	int32 interesting = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interst")
	int32 ObId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interst")
	ADmgDealer* manger;
	/** Updates the destination coordinate while dragging */
	UFUNCTION(BlueprintCallable, Category = "Movement|Physics")
	void UpdateDragLocation(FVector NewLocation);

	/** Releases the bone and stops tracking */
	UFUNCTION(BlueprintCallable, Category = "Movement|Physics")
	void StopDragging();
	bool bIsBeingDragged;
protected:
	// Component that handles the physics rope behavior
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPhysicsHandleComponent* PhysicsHandle;

private:
	
	FVector TargetDragLocation;
};
