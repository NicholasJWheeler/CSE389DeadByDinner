// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "Components/CapsuleComponent.h"
#include <vector>
#include "ControllableSurvivor.generated.h"

UCLASS()
class CSE389DEADBYDINNER_API AControllableSurvivor : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AControllableSurvivor();

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* CollectSound;

protected:
  // Used for querying survivors by zombies
  inline static std::vector<AControllableSurvivor*> Survivors = std::vector<AControllableSurvivor*>();

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

	// Collectible variable updated when we obtain chicken nuggets
	int CollectScore;

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

	// Tracking what weapons can be switched to
    bool OwnsUnarmed;
    bool OwnsPistol;
    bool OwnsShotgun;

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

	// SCORE

	// Set the score variable
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetScore(int NewScore);

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int ScoreToAdd);

	// Get the score variable
	UFUNCTION(BlueprintCallable, Category = "Score")
	int GetScore();

	// CHICKEN COLLECTED

	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetChickenCollected(int NewChickenCollected);

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddChickenCollected(int ChickenCollectedToAdd);

	UFUNCTION(BlueprintCallable, Category = "Score")
	int GetChickenCollected();

	// HEALTH

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

	// Weapon ownership/inventory
	
	// Get the value of if Unarmed is owned
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool GetOwnsUnarmed();

    // Set the value of if Unarmed is owned
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetOwnsUnarmed(bool Owned);

	// Get the value of if Pistol is owned
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool GetOwnsPistol();

    // Set the value of if Pistol is owned
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetOwnsPistol(bool Owned);

	// Get the value of if Shotgun is owned
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool GetOwnsShotgun();

    // Set the value of if Shotgun is owned
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetOwnsShotgun(bool Owned);

  // Get list of all Survivors in the game
  static const std::vector<AControllableSurvivor*>& GetSurvivorList();

  void DealDamage(int32 Damage, FString DamageSource);
};
