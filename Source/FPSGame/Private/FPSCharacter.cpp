// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "FPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimSequence.h"


AFPSCharacter::AFPSCharacter()
{
	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(0, 0, BaseEyeHeight)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1PComponent->SetupAttachment(CameraComponent);
	Mesh1PComponent->CastShadow = false;
	Mesh1PComponent->SetRelativeRotation(FRotator(2.0f, -15.0f, 5.0f));
	Mesh1PComponent->SetRelativeLocation(FVector(0, 0, -160.0f));

	// Create a gun mesh component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	GunMeshComponent->CastShadow = false;
	GunMeshComponent->SetupAttachment(Mesh1PComponent, "GripPoint");

	//Ä¬ÈÏÉ¢µ¯ÉèÖÃ
	bUseSpread = false;
	NumPellets = 6;
	SpreadAngleDegrees = 10.0f;

	//Ä¬ÈÏµ¯Ò©ÉèÖÃ
	MaxAmmoInClip = 30;
	CurrentAmmoInClip = MaxAmmoInClip;
	MaxReserveAmmo = 60;
	ReserveAmmo = MaxReserveAmmo;
	ReloadTimeSeconds = 2.0f;
	bIsReloading = false;
}


void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(Input_Move, ETriggerEvent::Triggered, this, &AFPSCharacter::MoveInput);
	EnhancedInputComponent->BindAction(Input_Look, ETriggerEvent::Triggered, this, &AFPSCharacter::LookInput);

	// Jump exists in the base class, we dont need our own function
	EnhancedInputComponent->BindAction(Input_Jump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(Input_Fire, ETriggerEvent::Triggered, this, &AFPSCharacter::Fire);

	//°ó¶¨»»µ¯
	if (Input_Reload)
	{
		EnhancedInputComponent->BindAction(Input_Reload, ETriggerEvent::Triggered, this, &AFPSCharacter::StartReload);
	}

	const APlayerController* PC = GetController<APlayerController>();
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	// Add mappings for our game, more complex games may have multiple Contexts that are added/removed at runtime
	Subsystem->AddMappingContext(DefaultInputMapping, 0);
}


void AFPSCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (IsLocallyControlled())
	{
		/* Play landed camera anim */
		APlayerController* PC = Cast<APlayerController>(GetController());
		PC->PlayerCameraManager->StartCameraShake(LandedCameraShake);
		
		//UGameplayStatics::PlaySound2D(this, LandedSound);
	}
}

void AFPSCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	// Don't play when this code is running for another player's character (multiplayer)
	if (IsLocallyControlled())
	{
		/* Play jump camera anim */
		APlayerController* PC = Cast<APlayerController>(GetController());
		PC->PlayerCameraManager->StartCameraShake(JumpCameraShake);
		
		//UGameplayStatics::PlaySound2D(this, JumpedSound);
	}
}


void AFPSCharacter::Fire()
{
	if (bIsReloading) return;  //»»µ¯Ê±²»ÄÜ¿ª»ð

	if (CurrentAmmoInClip <= 0) {

		if (ReserveAmmo > 0) StartReload();

		return;
	}
	
	// try and fire a projectile
	if (ProjectileClass)
	{
		// Grabs location from the mesh that must have a socket called "Muzzle" in his skeleton
		FVector MuzzleLocation = GunMeshComponent->GetSocketLocation("Muzzle");
		// Use controller rotation which is our view direction in first person
		FRotator MuzzleRotation = GetControlRotation();

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = this;

		if (bUseSpread && NumPellets > 1) {
			const float HalfAngle = SpreadAngleDegrees * 0.5f;
			for (int32 i = 0; i < NumPellets; i++) {
				float YawOff = FMath::RandRange(-HalfAngle, HalfAngle);
				float PitchOff = FMath::RandRange(-HalfAngle, HalfAngle);
				FRotator SpreadRot = MuzzleRotation + FRotator(PitchOff, YawOff, 0.0f);

				/*×Óµ¯Î»ÖÃËæ»ú*/
				FVector ForwardDir = MuzzleRotation.Vector();
				FVector RightDir = FRotationMatrix(MuzzleRotation).GetScaledAxis(EAxis::Y);
				FVector UpDir = FRotationMatrix(MuzzleRotation).GetScaledAxis(EAxis::Z);
				FVector OffsetLocation = MuzzleLocation
					+ RightDir * FMath::RandRange(-2.0f, 2.0f)
					+ UpDir * FMath::RandRange(-2.0f, 2.0f);

				AFPSProjectile* Proj = GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, OffsetLocation, SpreadRot, ActorSpawnParams);
				UE_LOG(LogTemp, Warning, TEXT("Spawned pellet %d, ptr=%s"), i, (Proj ? TEXT("Valid") : TEXT("NULL")));
			}
		}
		else {
			// spawn the projectile at the muzzle
			GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
		}
		
		--CurrentAmmoInClip;
	}

	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	
	// Get the animation object for the arms mesh
	UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
	}

	// Play Muzzle FX
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GunMeshComponent, "Muzzle");
}

void AFPSCharacter::MoveInput(const FInputActionValue& InputValue)
{
	// Combined input from forward/back (X) and left/right (Y)
	FVector2d MoveValue = InputValue.Get<FVector2d>();
	
	// add movement in that direction
	AddMovementInput(GetActorForwardVector(), MoveValue.X);

	// add movement in that direction
	AddMovementInput(GetActorRightVector(), MoveValue.Y);
}

void AFPSCharacter::LookInput(const FInputActionValue& InputValue)
{
	// Combined input from look up/down (X) and left/right (Y)
	FVector2d LookValue = InputValue.Get<FVector2d>();
	
	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AFPSCharacter::StartReload() {
	if (bIsReloading || CurrentAmmoInClip >= MaxAmmoInClip || ReserveAmmo <= 0) return;

	bIsReloading = true;

	//²¥·Å»»µ¯ÒôÆµ
	if (ReloadSound) UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());

	UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
	if (AnimInstance && ReloadAnimation)
	{
		AnimInstance->PlaySlotAnimationAsDynamicMontage(ReloadAnimation, "Arms", 0.0f);
	}

	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AFPSCharacter::FinishReload, ReloadTimeSeconds);
}

void AFPSCharacter::FinishReload()
{
	if (!bIsReloading)
		return;

	int32 Need = MaxAmmoInClip - CurrentAmmoInClip;
	int32 ToReload = FMath::Min(Need, ReserveAmmo);

	CurrentAmmoInClip += ToReload;
	ReserveAmmo -= ToReload;

	bIsReloading = false;
}