// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "Components/CapsuleComponent.h"
#include "ControllableSurvivor.generated.h"

UCLASS()
class CSE389DEADBYDINNER_API AControllableSurvivor : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AControllableSurvivor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* InputMove;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* InputLook;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* InputShoot;

	// Bullet spawn location from the ControllableCharacter
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ProjectileSpawn;

	UCapsuleComponent* CollisionComp;

	// Score variable updated when we shoot an enemy
	int Score;

	// Health variable updated when we get hit by an 'enemy'
	int32 Health;

	// Collectables variable updated when we pick up food collectables in the world
	int ChickenCollected;

	// Ammo variables updated when switching guns/shooting/reloading
	int PistolLoadedAmmo;
	int PistolReserveAmmo;
	int ShotgunLoadedAmmo;
	int ShotgunReserveAmmo;

	// Tracking what weapon is equipped
	bool UnarmedEquipped;
	bool PistolEquipped;
	bool ShotgunEquipped;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// Function that handles firing projectiles
	void Shoot();

	// Set the score variable
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetScore(int NewScore);

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int ScoreToAdd);

	// Get the score variable
	UFUNCTION(BlueprintCallable, Category = "Score")
	int GetScore();

	// Get the health variable
	UFUNCTION(BlueprintCallable, Category = "Health")
	int GetHealth();

	// Subtract from health variable
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SubtractHealth(int HealthToSubtract);

	// AMMO FUNCTIONS

	// Get the amount of pistol ammo in reserve
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int GetPistolReserveAmmo();

	// Set the amount of pistol ammo in reserve
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void SetPistolReserveAmmo(int ReserveAmmo);

	// Get the amount of shotgun ammo in reserve
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int GetShotgunReserveAmmo();

	// Set the amount of shotgun ammo in reserve
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void SetShotgunReserveAmmo(int ReserveAmmo);

	// Get the amount of shotgun ammo loaded
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int GetShotgunLoadedAmmo();

	// Set the amount of shotgun ammo loaded
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void SetShotgunLoadedAmmo(int LoadedAmmo);

	// Get the amount of pistol ammo loaded
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int GetPistolLoadedAmmo();

	// Set the amount of pistol ammo loaded
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void SetPistolLoadedAmmo(int LoadedAmmo);

	// WEAPON FUNCTIONS

	// Get the value of if Unarmed is equipped
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetUnarmedEquipped();

	// Set the value of if Unarmed is equipped
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetUnarmedEquipped(bool Equipped);

	// Get the value of if Pistol is equipped
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetPistolEquipped();

	// Set the value of if Pistol is equipped
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetPistolEquipped(bool Equipped);

	// Get the value of if Shotgun is equipped
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetShotgunEquipped();

	// Set the value of if Shotgun is equipped
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetShotgunEquipped(bool Equipped);

};
