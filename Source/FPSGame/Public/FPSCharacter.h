// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// Included for struct FInputActionInstance (Enhanced Input)
#include "InputAction.h"

#include "FPSCharacter.generated.h"

class UInputMappingContext;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class AFPSProjectile;
class USoundBase;
class UAnimSequence;
class UParticleSystem;


UCLASS()
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	// -- Enhanced Input -- //

	/* Holds collection of currently active and available InputActions */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputMappingContext* DefaultInputMapping;

	UPROPERTY(EditDefaultsOnly, Category= "Input")
	UInputAction* Input_Move;
	
	UPROPERTY(EditDefaultsOnly, Category= "Input")
	UInputAction* Input_Look;
	
	UPROPERTY(EditDefaultsOnly, Category= "Input")
	UInputAction* Input_Jump;
	
	UPROPERTY(EditDefaultsOnly, Category= "Input")
	UInputAction* Input_Fire;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* Input_Reload;

	/** Pawn mesh: 1st person view  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh1PComponent;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* GunMeshComponent;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> LandedCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> JumpCameraShake;

public:
	AFPSCharacter();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<AFPSProjectile> ProjectileClass;

	/*是否启用散射设置*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bUseSpread;
	/*散射子弹数量*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 NumPellets;
	/*散射角度*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float SpreadAngleDegrees;

	/*子弹系统*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxAmmoInClip;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 CurrentAmmoInClip;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxReserveAmmo;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 ReserveAmmo;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ReloadTimeSeconds;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	USoundBase* FireSound;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	USoundBase* ReloadSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimSequence* FireAnimation;
	/*换弹动作*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimSequence* ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	UParticleSystem* MuzzleFlash;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnJumped_Implementation() override;

protected:
	
	/** Fires a projectile. */
	void Fire();

	void StartReload(); //开始换弹
	void FinishReload(); //结束换弹

	void MoveInput(const FInputActionValue& InputValue);
	void LookInput(const FInputActionValue& InputValue);

	FTimerHandle ReloadTimerHandle;
	bool bIsReloading;
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComponent; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return CameraComponent; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmoInClip; }
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetReserveAmmo() const { return ReserveAmmo; }
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsReloading() const { return bIsReloading; }

};

