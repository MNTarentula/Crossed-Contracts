// Copyright Epic Games, Inc. All Rights Reserved.

#include "K7Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"
#include "K7Npc.h"
#include "K7RangedWeapons.h"
#include "PistolCOM.h"
#include "Blueprint/UserWidget.h"
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AK7Character

AK7Character::AK7Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	PrimaryActorTick.bCanEverTick = true;
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 175.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	onlyT = false;
}
void AK7Character::BeginPlay()
{
	// 1. Let the parent classes run their BeginPlay first
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("PickupAction = %s"), *GetNameSafe(PickUPAction));
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && WidCla)
	{
		// 2. Spawn/Create the widget but do not add to viewport yet
		WidIns = CreateWidget<UUserWidget>(PC, WidCla);

		if (WidIns)
		{
			// 3. Add to screen
			WidIns->AddToViewport();

			// 4. Initially hide it so it doesn't show at spawn
			WidIns->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	//// 2. Setup standard spawn parameters
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = this;
	//SpawnParams.Instigator = Cast<APawn>(this);

	//// 3. HARDCODED SPAWN: Spawns the raw C++ Pistol class directly at your feet!
	//APistolCOM* TestPistol = GetWorld()->SpawnActor<APistolCOM>(
	//	APistolCOM::StaticClass(),
	//	GetActorLocation(),
	//	GetActorRotation(),
	//	SpawnParams
	//);

	//// 4. Force it straight into your inherited combat system
	//if (TestPistol)
	//{
	//	CollectWeapon(TestPistol);
	//	UE_LOG(LogTemp, Warning, TEXT("TESTING HACK: Hardcoded C++ Pistol successfully spawned and equipped!"));
	//}
}
//////////////////////////////////////////////////////////////////////////
// Input
void AK7Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator TargetRotation = FRotator::ZeroRotator;
	FRotator ControlRot = GetControlRotation();
	FRotator ActorRot = GetActorRotation();

	// Get the relative difference
	TargetRotation = (ControlRot - ActorRot).GetNormalized();

	TargetRotation.Yaw = FMath::Clamp(TargetRotation.Yaw, -MaxLookAngle, MaxLookAngle);
	TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, -MaxLookAngle, MaxLookAngle);
	TargetRotation.Roll = 0.0f; // Necks don't usually roll much sideways while tracking

	// Smoothly interpolate from current head rotation to the target rotation
	RotatorHead = FMath::RInterpTo(RotatorHead, TargetRotation, DeltaTime, HeadInterpSpeed);
	
	if (shotPos > 0)
	{
		// COMBAT MODE: Turn off Unreal's automated systems so they don't fight us
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->MaxWalkSpeed = 250.f;

		// Create a target rotation using ONLY the Camera's Yaw (Keep Pitch/Roll flat)
		FRotator TargetCapsuleRot = FRotator(0.0f, ControlRot.Yaw, 0.0f);

		// By default, we want to rotate (handles hip fire shotPos == 1)
		bool bShouldRotateCapsule = true;
		CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 20.0f);
		
		// SMART CHECK: If we are in ADS/Aim mode, look for the 45-degree deadzone, removed for time reason more animations bug that need to fix i fuck to fix that shit fuck your animation and all 3d who create it, why so wierd why i need do it aaaaaaa
		//if (shotPos == 2)
		//{
		//	float AngleDifference = FRotator::NormalizeAxis(ControlRot.Yaw - ActorRot.Yaw);

		//	if (FMath::Abs(AngleDifference) <= 45.0f)
		//	{
		//		// Inside the 45-degree cone? Keep the legs/capsule completely still
		//		bShouldRotateCapsule = false;
		//	}
		//	else
		//	{
		//		// SMOOTH CONE DRAG: Target the EDGE of the cone, not the camera center!
		//		if (AngleDifference > 45.0f)
		//		{
		//			// Camera is too far right, pull capsule right to maintain the 45-degree limit
		//			TargetCapsuleRot.Yaw = FRotator::NormalizeAxis(ControlRot.Yaw - 45.0f);
		//		}
		//		else if (AngleDifference < -45.0f)
		//		{
		//			// Camera is too far left, pull capsule left to maintain the 45-degree limit
		//			TargetCapsuleRot.Yaw = FRotator::NormalizeAxis(ControlRot.Yaw + 45.0f);
		//		}
		//	}
		//}

		// Only run your smooth interpolation if we break the threshold or are hip-firing
		if (bShouldRotateCapsule)
		{
			// Smoothly rotate the entire actor toward the camera direction
			// 12.0f is the rotation speed. Make it higher for faster turns, lower for slower turns!
			FRotator NewRotation = FMath::RInterpTo(ActorRot, TargetCapsuleRot, DeltaTime, 12.0f);
			SetActorRotation(NewRotation);
			
		}
		
	}
	else
	{
		// RELAXED/PATROL MODE: Return to default third-person free camera movement
		bUseControllerRotationYaw = false;
		CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	
	}
	if (WeaponType > 0 && !onlyT) {
			
			WidIns->SetVisibility(ESlateVisibility::Visible);
			onlyT = true;
			
	}
	else if (WeaponType == 0 && onlyT) {
			WidIns->SetVisibility(ESlateVisibility::Hidden);
			onlyT = false;
	}
	switch (drugstate)
	{
	case 0:
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
		break;

	default:
		GetCharacterMovement()->MaxWalkSpeed = 250.f;
		break;
	}
}
void AK7Character::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AK7Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AK7Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AK7Character::Look);
		//shoot
		EnhancedInputComponent->BindAction(
			ShootAction,
			ETriggerEvent::Triggered,
			this,
			&AK7Character::Shoot
		);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AK7Character::AimR);
		//PickUP
		EnhancedInputComponent->BindAction(
			PickUPAction,
			ETriggerEvent::Started,
			this,
			&AK7Character::PickUP
		);
		EnhancedInputComponent->BindAction(
			switchAction1,
			ETriggerEvent::Started,
			this,
			&AK7Character::swit1
		);
		EnhancedInputComponent->BindAction(
			switchAction2,
			ETriggerEvent::Started,
			this,
			&AK7Character::swit2
		);
		EnhancedInputComponent->BindAction(
			switchAction3,
			ETriggerEvent::Started,
			this,
			&AK7Character::swit3
		);
		EnhancedInputComponent->BindAction(
			switchAction4,
			ETriggerEvent::Started,
			this,
			&AK7Character::swit4
		);
		EnhancedInputComponent->BindAction(
			ReloadAction,
			ETriggerEvent::Started,
			this,
			&AK7Character::reload
		);
		EnhancedInputComponent->BindAction(
			shiftAct,
			ETriggerEvent::Started,
			this,
			&AK7Character::shit
		);
		EnhancedInputComponent->BindAction(
			Tact,
			ETriggerEvent::Started,
			this,
			&AK7Character::tish
		);
	
	}else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void AK7Character::swit1(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		SwitchWeapon(0);
		stopDra();
	}
}void AK7Character::swit2(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		SwitchWeapon(1);
		stopDra();
	}
}void AK7Character::swit3(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		SwitchWeapon(2);
		stopDra();
	}
}void AK7Character::swit4(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		SwitchWeapon(3);
		stopDra();
	}
}

void AK7Character::shit(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		shotPos = 0;
		stopDra();

	}
}
void AK7Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
void AK7Character::Shoot(const FInputActionValue& Value)
{
	if (Controller != nullptr && WeaponType > 0 && secret == 0)
	{
		if (shotPos != 2) {
			shotPos = 1;
		}
		TestTrace();
		ResetCombatCooldown();
		float UpwardKick = CurrentRangedData.Recoil * 2;

		// Randomize horizontal kick slightly so it goes left/right a bit
		float SideKick = FMath::FRandRange(-0.02f, 0.02f);
		AddControllerPitchInput(-UpwardKick); // Checks your inversion settings, usually negative or positive depending on project setup
		AddControllerYawInput(SideKick);
		niVezde();
	}
}
void AK7Character::reload(const FInputActionValue& Value)
{
	if (Controller != nullptr && WeaponType > 0)
	{
		ReloadCurrentWeapon();
	}
}
void AK7Character::AimR(const FInputActionValue& Value)
{
	if (!Controller || !CameraBoom) return;

	
	if (WeaponType > 0) {
		if (shotPos != 2)
		{
			StartAim(); // Call your base combat logic
			CameraBoom->TargetArmLength = 100.0f; // Zoom in
			CameraBoom->SocketOffset = FVector(0.0f, 25.0f, 20.0f);
			GetWorldTimerManager().ClearTimer(RepeatingTimerHandle);
			// Note: You might want to slow down player movement speed here too!
		}
		else
		{
			StopAim(); // Call your base combat logic
			CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
			CameraBoom->TargetArmLength = 175.0f; // Zoom back out
			ResetCombatCooldown();
		}
	}
}
void AK7Character::PickUP(const FInputActionValue& Value)
{

	if (Controller != nullptr)
	{
		//stopDra(); fuck do problem in teaory need for checks better i do that in weapons if they equiped do stopDra
		FVector Start = FollowCamera->GetComponentLocation();
		FVector Direction = FollowCamera->GetForwardVector();
		FVector End = Start + (Direction * 1000.f);
		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		/*if (cloth){QueryParams.AddIgnoredActor(cloth);}*/
		FCollisionShape MySphere = FCollisionShape::MakeSphere(175.0f);

		GetWorld()->SweepSingleByChannel(
			Hit,
			Start,
			End,
			FQuat::Identity,
			ECC_Visibility,
			MySphere,
			QueryParams
		);
		TryPickupWeapon(Hit);
		getThemDo(Hit);
		changeC(Hit);
	}
}
void AK7Character::tish(const FInputActionValue& Value)
{

	if (Controller != nullptr)
	{
		//stopDra(); fuck do problem in teaory need for checks better i do that in weapons if they equiped do stopDra
		FVector Start = FollowCamera->GetComponentLocation();
		FVector Direction = FollowCamera->GetForwardVector();
		FVector End = Start + (Direction * 1000.f);
		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		/*if (cloth){QueryParams.AddIgnoredActor(cloth);}*/
		FCollisionShape MySphere = FCollisionShape::MakeSphere(175.0f);

		GetWorld()->SweepSingleByChannel(
			Hit,
			Start,
			End,
			FQuat::Identity,
			ECC_Visibility,
			MySphere,
			QueryParams
		);
		redressN(Hit);
	}
}
void AK7Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void AK7Character::TestTrace()
{
	if (!FollowCamera) return;

	// 1. Grab the camera's location and forward angle vectors
	FVector Start = FollowCamera->GetComponentLocation();
		//- FollowCamera->GetRightVector() * (CameraBoom->SocketOffset.Y * 0.5);
	FVector Direction = FollowCamera->GetForwardVector();

	// 2. Call the parent Fire() function from K7CombatBase!
	// This automatically handles Ammo checks, drops ammo count, 
	// extracts the weapon range, shoots the trace, and draws your debug lines.
	FHitResult Hit = Fire(Start, Direction);

	// 3. If the parent weapon trace actually collided with a target:
	if (Hit.bBlockingHit && Hit.GetActor())
	{
		// Keep your awesome tracking debug logs intact!
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s | Component: %s"), *Hit.GetActor()->GetName(), *Hit.GetComponent()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Hit Bone: %s"), *Hit.BoneName.ToString());

		// 4. Check if we shot an NPC base target
		AK7Npc* Npc = Cast<AK7Npc>(Hit.GetActor());
		if (Npc)
		{
			// Instead of a hardcoded '20', pass your gun's real struct damage stat!
			// CurrentRangedData is inherited straight out of your K7CombatBase parent
			float DynamicDamage = CurrentRangedData.Damage;

			// Deal damage dynamically based on the current weapon held!
			Npc->getDamgetf(DynamicDamage, Hit);
		}
	}
}