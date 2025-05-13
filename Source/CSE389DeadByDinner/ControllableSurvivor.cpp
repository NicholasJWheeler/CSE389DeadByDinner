// Fill out your copyright notice in the Description page of Project Settings.


#include "ControllableSurvivor.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/LogMacros.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Math/Plane.h" 
#include "Math/UnrealMathUtility.h" // for cursor position rotation
#include "EnhancedInputComponent.h"
#include "CSE389DeadByDinner/BasicProjectile.h"
#include "CSE389DeadByDinner/BasicProjectile.h"

// Sets default values
AControllableSurvivor::AControllableSurvivor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Variables
	Score = 0; // Start with no points (skulls = zombies killed)
	ChickenCollected = 0; // Start with no food collected
	Health = 100; // Start with full health
	PistolLoadedAmmo = 0; // Start with some pistol ammo (MAX 9) OR no ammo at all
	PistolReserveAmmo = 0; // Start with some reserve pistol ammo (MAX 180) OR no ammo at all
	ShotgunLoadedAmmo = 0; // Start with some shotgun shells (MAX 5) OR no ammo at all
	ShotgunReserveAmmo = 0; // Start with some reserve shells (MAX 60) OR no ammo at all

	// Starting weapon is nothing
	UnarmedEquipped = true;
	PistolEquipped = false;
	ShotgunEquipped = false;

	// Starting weapons owned/accessible
    OwnsUnarmed = true;
    OwnsPistol = false;
    OwnsShotgun = false;

	// Getting reference to Bullet BP
    static ConstructorHelpers::FClassFinder<ABasicProjectile> PistolProjBP(
        TEXT("/Game/Blueprints/BP_PistolProjectile"));
    if (PistolProjBP.Class != nullptr)
        PistolProjClass = PistolProjBP.Class;
    static ConstructorHelpers::FClassFinder<ABasicProjectile> ShotgunProjBP(
        TEXT("/Game/Blueprints/BP_ShotgunProjectile"));
    if (ShotgunProjBP.Class != nullptr)
        ShotgunProjClass = ShotgunProjBP.Class;
}

// Called when the game starts or when spawned
void AControllableSurvivor::BeginPlay()
{
	Super::BeginPlay();
	
	// For controlling when something hits the character
	CollisionComp = FindComponentByClass<UCapsuleComponent>();

	if (CollisionComp)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &AControllableSurvivor::OnHit);
	}

	Survivors.push_back(this);

	// Reloading status intialization
    bIsReloading = false;
    ReloadPercentage = 100.0f; // Start fully loaded

    // Load sound assets
    PistolReloadSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/GunSounds/PistolReload.PistolReload"));
    ShotgunReloadSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/GunSounds/ShotgunReload.ShotgunReload"));

}

// Called every frame
void AControllableSurvivor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// LOOK LOGIC HERE (needed to update every frame so character aims towards reticle)
	if (Controller != nullptr)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (PlayerController != nullptr)
		{
			// Get mouse position in screen space
			FVector2D MousePosition;
			if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
			{
				// Convert mouse position to world space
				FVector WorldPosition;
				FVector WorldDirection;
				if (PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldPosition, WorldDirection))
				{
					// plane at the character's Z height for proper intersection
					const FVector CharacterLocation = GetActorLocation();
					const FVector PlaneOrigin(0, 0, CharacterLocation.Z);
					const FVector PlaneNormal(0, 0, 1); // World up vector

					// Create the plane using origin and normal
					FPlane Plane(PlaneOrigin, PlaneNormal);

					// The line is defined by two points
					FVector IntersectionPoint = FMath::LinePlaneIntersection(
						WorldPosition,
						WorldPosition + WorldDirection * 10000.0f,
						Plane
					);

					// Calculate direction from character to intersection point (ignoring Z)
					FVector ToTarget = IntersectionPoint - CharacterLocation;
					ToTarget.Z = 0;

					// Only rotate if we have a valid direction
					if (!ToTarget.IsNearlyZero())
					{
						// Convert to rotation
						FRotator NewRotation = ToTarget.Rotation();

						// Set the actor's rotation to face that point
						SetActorRotation(NewRotation);
					}
				}
			}
		}
	}

	if (bIsReloading)
        {
            float reloadTime = PistolEquipped ? PistolReloadTime : ShotgunReloadTime;
            ReloadPercentage += (100.0f / reloadTime) * DeltaTime;

            if (ReloadPercentage >= 100.0f)
            {
                ReloadPercentage = 100.0f;
                bIsReloading = false;

                // Once reload is complete, move ammo from reserve to loaded
                if (PistolEquipped)
                {
                    int ammoNeeded = MAX_PISTOL_LOADED_AMMO - PistolLoadedAmmo;
                    int ammoAvailable = FMath::Min(ammoNeeded, PistolReserveAmmo);

                    PistolLoadedAmmo += ammoAvailable;
                    PistolReserveAmmo -= ammoAvailable;
                }
                else if (ShotgunEquipped)
                {
                    int ammoNeeded = MAX_SHOTGUN_LOADED_AMMO - ShotgunLoadedAmmo;
                    int ammoAvailable = FMath::Min(ammoNeeded, ShotgunReserveAmmo);

                    ShotgunLoadedAmmo += ammoAvailable;
                    ShotgunReserveAmmo -= ammoAvailable;
                }
            }
        }

}

// Called to bind functionality to input
void AControllableSurvivor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get the player controller
	APlayerController* PC = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	// Get the EnhancedInputComponent
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// Bind the actions
	PEI->BindAction(InputMove, ETriggerEvent::Triggered, this, &AControllableSurvivor::Move);
	PEI->BindAction(InputLook, ETriggerEvent::Triggered, this, &AControllableSurvivor::Look);
	PEI->BindAction(InputShoot, ETriggerEvent::Started, this, &AControllableSurvivor::Shoot);

}

void AControllableSurvivor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Need to probably change this to get accurate player damage taken
	if (OtherComponent->GetCollisionProfileName().ToString() == "Enemy") {
		Health -= 10;
		UE_LOG(LogTemp, Warning, TEXT("Other component is enemy!"), Health);
	}

	// UE_LOG(LogTemp, Warning, TEXT("Health: %d"), Health);
}

void AControllableSurvivor::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		// Forward/Backward direction
		if (MoveValue.Y != 0.f)
		{
			// Get forward vector
			const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);

			AddMovementInput(Direction, MoveValue.Y);
		}

		// Right/Left direction
		if (MoveValue.X != 0.f)
		{
			// Get right vector
			const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);

			AddMovementInput(Direction, MoveValue.X);
		}
	}
}

void AControllableSurvivor::Look(const FInputActionValue& Value)
{
	// Make the player character rotate to face the current mouse cursor position in the world/screen positioning. (Ignore buildings and such and just look at the cursor placement)

	// (we don't actually need this as the logic has been moved to the Tick function)
	
}

// Spawn in unique projectiles based on if pistol or shotgun is equipped
void AControllableSurvivor::Shoot()
{

    // If reloading, don't allow shooting
    if (bIsReloading)
    {
        return;
    }

    // Don't do anything if unarmed
    if (UnarmedEquipped)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Unarmed is equipped, so do nothing!"));
        return;
    }

    // Set correct offsets from player for gun muzzle flashes
    FVector PistolMuzzleOffset(112.0f, 8.0f, 26.0f);  // Forward, Right, Up
    FVector ShotgunMuzzleOffset(98.0f, 10.0f, 20.0f); // Forward, Right, Up

    // Get the character's rotation
    FRotator CharacterRotation = GetActorRotation();

    // PISTOL SHOOTING
    if (PistolEquipped)
    {
        if (PistolLoadedAmmo > 0)
        {
            // Spawn pistol muzzle flash effect
            UNiagaraSystem *PistolMuzzleFlash = LoadObject<UNiagaraSystem>(
                nullptr, TEXT("/Game/MuzzleFlash/MuzzleFlash/Niagara/NS_PistolMuzzleFlash.NS_PistolMuzzleFlash"));

            if (PistolMuzzleFlash)
            {
                // Transform the offset from local to world space
                FVector WorldOffset = CharacterRotation.RotateVector(PistolMuzzleOffset);

                // Calculate final world position
                FVector WorldPosition = GetActorLocation() + WorldOffset;
                FRotator EmitterRotation = CharacterRotation;
                EmitterRotation.Yaw -= 90.0f; // Rotate -90 degrees around Z axis to align with forward since emitter
                                              // doesn't shoot down forward vector

                // Create a muzzle flash niagara particle emitter at the world location with the adjusted rotation
                UNiagaraComponent *NiagaraComp =
                    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PistolMuzzleFlash,
                                                                   WorldPosition,      // World position
                                                                   EmitterRotation,    // Adjusted rotation
                                                                   FVector(1.0f),      // Scale
                                                                   true,               // Auto destroy
                                                                   true,               // Auto activate
                                                                   ENCPoolMethod::None // Pool method
                    );

                if (NiagaraComp)
                {
                    float EffectLifetime = 0.24f; // 0.24 seconds until end of particle effect
                    FTimerHandle TimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(
                        TimerHandle,
                        [NiagaraComp]()
                        {
                            if (NiagaraComp && !NiagaraComp->IsBeingDestroyed())
                            {
                                NiagaraComp->DeactivateImmediate();
                                NiagaraComp->DestroyComponent();
                            }
                        },
                        EffectLifetime, false);
                }
            }

		if (PistolProjClass != nullptr)
        {
            // Transform the offset from local to world space
            FVector WorldOffset = CharacterRotation.RotateVector(PistolMuzzleOffset);

            // Calculate final world position
            FVector WorldPosition = GetActorLocation() + WorldOffset;
            FRotator BulletRotation = CharacterRotation;

            ABasicProjectile *Bullet =
                GetWorld()->SpawnActor<ABasicProjectile>(PistolProjClass, WorldPosition, BulletRotation);

            if (Bullet)
            {
                Bullet->Fire(this, BulletRotation);
            }
        }

            // Play pistol sound safely with cooldown to prevent audio overload
            static float LastPistolSoundTime = 0.0f;
            float CurrentTime = GetWorld()->GetTimeSeconds();
            if (CurrentTime - LastPistolSoundTime > 0.1f) // 0.1 second cooldown between sounds
            {
                USoundBase *PistolSound =
                    LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/GunSounds/PistolShooting.PistolShooting"));
                if (PistolSound)
                {
                    UGameplayStatics::PlaySound2D(GetWorld(), PistolSound, 0.7f, 1.0f, 0.0f);
                    LastPistolSoundTime = CurrentTime;
                }
            }

            // Decrease ammo
            PistolLoadedAmmo--;

            if (PistolLoadedAmmo == 0 && PistolReserveAmmo > 0)
            {
                Reload(); // Auto-reload when empty (tf2 feature)
            }
        }
        else if (PistolReserveAmmo > 0)
        {
            // Reload if player tries to shoot on empty clip
            Reload();
        }
    }
    
    
    // SHOTGUN SHOOTING
    else if (ShotgunEquipped)
    {
        if (ShotgunLoadedAmmo > 0)
        {
            // Spawn shotgun muzzle flash effect
            UNiagaraSystem *ShotgunMuzzleFlash = LoadObject<UNiagaraSystem>(
                nullptr, TEXT("/Game/MuzzleFlash/MuzzleFlash/Niagara/NS_ShotgunMuzzleFlash.NS_ShotgunMuzzleFlash"));

            if (ShotgunMuzzleFlash)
            {
                // Transform the offset from local to world space
                FVector WorldOffset = CharacterRotation.RotateVector(ShotgunMuzzleOffset);

                // Calculate final world position
                FVector WorldPosition = GetActorLocation() + WorldOffset;
                FRotator EmitterRotation = CharacterRotation;
                EmitterRotation.Yaw -= 90.0f; // Rotate -90 degrees around Z axis to align with forward since emitter
                                              // doesn't shoot down forward vector

                // Create a muzzle flash niagara particle emitter at the world location with the adjusted rotation
                UNiagaraComponent *NiagaraComp =
                    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShotgunMuzzleFlash,
                                                                   WorldPosition,      // World position
                                                                   EmitterRotation,    // Adjusted rotation
                                                                   FVector(1.0f),      // Scale
                                                                   true,               // Auto destroy
                                                                   true,               // Auto activate
                                                                   ENCPoolMethod::None // Pool method
                    );

                if (NiagaraComp)
                {
                    float EffectLifetime = 0.24f; // 0.24 seconds until end of particle effect
                    FTimerHandle TimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(
                        TimerHandle,
                        [NiagaraComp]()
                        {
                            if (NiagaraComp && !NiagaraComp->IsBeingDestroyed())
                            {
                                NiagaraComp->DeactivateImmediate();
                                NiagaraComp->DestroyComponent();
                            }
                        },
                        EffectLifetime, false);
                }
            }

		if (ShotgunProjClass != nullptr)
        {
            // Transform the offset from local to world space
            FVector WorldOffset = CharacterRotation.RotateVector(ShotgunMuzzleOffset);

            // Calculate final world position
            FVector WorldPosition = GetActorLocation() + WorldOffset;
            FRotator BaseRotation = CharacterRotation;

            int NumShots = 5;
            std::vector<ABasicProjectile *> Shots(NumShots);

            float SpreadAngle = 20.0f; // Degrees

            for (int i = 0; i < NumShots; i++)
            {
                // Random yaw and pitch offset within [-SpreadAngle/2, +SpreadAngle/2]
                float YawOffset = FMath::FRandRange(-SpreadAngle / 2.0f, SpreadAngle / 2.0f);
                //float PitchOffset = FMath::FRandRange(-SpreadAngle / 2.0f, SpreadAngle / 2.0f);
                FRotator SpreadRotation = BaseRotation.Add(0.0f, YawOffset, 0.0f); // Roll stays unchanged

                ABasicProjectile *Shot =
                    GetWorld()->SpawnActor<ABasicProjectile>(ShotgunProjClass, WorldPosition, SpreadRotation);
                if (Shot)
                {
                    Shot->Fire(this, SpreadRotation);
                }
            }
        }

            // Play shotgun sound safely with cooldown to prevent audio overload
            static float LastShotgunSoundTime = 0.0f;
            float CurrentTime = GetWorld()->GetTimeSeconds();
            if (CurrentTime - LastShotgunSoundTime > 0.15f) // 0.15 second cooldown between sounds
            {
                USoundBase *ShotgunSound =
                    LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/GunSounds/ShotgunShoot.ShotgunShoot"));
                if (ShotgunSound)
                {
                    UGameplayStatics::PlaySound2D(GetWorld(), ShotgunSound, 0.7f, 1.0f, 0.0f);
                    LastShotgunSoundTime = CurrentTime;
                }
            }

            // Decrease ammo
            ShotgunLoadedAmmo--;

            if (ShotgunLoadedAmmo == 0 && ShotgunReserveAmmo > 0)
            {
                Reload(); // Auto-reload when empty
            }
        }
        else if (ShotgunReserveAmmo > 0)
        {
            // If trigger pulled with empty shotgun and reserve shells are owned, start reloading
            Reload();
        }
    }

 
	// PROJECTILES STUFF
	// If pistol equipped, shoot 1 bullet per click, spawn small explosion niagra effect at gun barrel
	// If shotgun equipped, shoot 5 bullets in a short range star pattern, spawn cone niagra explosion effect at gun barrel


	// Set ProjectileSpawn to spawn projectiles slightly in front of the character
	//ProjectileSpawn.Set(100.0f, 0.0f, 0.0f);

	//// Transform ProjectileSpawn from local space to world space.
	//FVector SpawnLocation = GetActorLocation() + FTransform(GetActorRotation()).TransformVector(ProjectileSpawn);

	//// Skew the aim to be slightly upwards.
	//FRotator SpawnRotation = GetActorRotation();
	//SpawnRotation.Pitch += 10.0f;

	//// Give our projectile some metadata
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = this;
	//SpawnParams.Instigator = GetInstigator();

	//// Spawn the projectile using our locations and metadata
	//AProjectile* Bullet = GetWorld()->SpawnActor<AProjectile>(SpawnLocation, SpawnRotation, SpawnParams);

	//if (Bullet)
	//{
	//	// Set the projectile's initial trajectory.
	//	FVector LaunchDirection = SpawnRotation.Vector();
	//	Bullet->FireInDirection(LaunchDirection);
	//}
}



int AControllableSurvivor::GetScore()
{
	return Score;
}

int AControllableSurvivor::GetChickenCollected()
{
	return ChickenCollected;
}

int AControllableSurvivor::GetHealth()
{
	return Health;
}

void AControllableSurvivor::SubtractHealth(int HealthToSubtract)
{
	Health -= HealthToSubtract; }

void AControllableSurvivor::SetHealth(int NewHealth) 
{ 
    Health = NewHealth; 
}


// Ammo Management

int AControllableSurvivor::GetPistolReserveAmmo()
{
	return PistolReserveAmmo;
}

void AControllableSurvivor::SetPistolReserveAmmo(int ReserveAmmo)
{
	PistolReserveAmmo = ReserveAmmo;
}

int AControllableSurvivor::GetShotgunReserveAmmo()
{
	return ShotgunReserveAmmo;
}

void AControllableSurvivor::SetShotgunReserveAmmo(int ReserveAmmo)
{
	ShotgunReserveAmmo = ReserveAmmo;
}

int AControllableSurvivor::GetShotgunLoadedAmmo()
{
	return ShotgunLoadedAmmo;
}

void AControllableSurvivor::SetShotgunLoadedAmmo(int LoadedAmmo)
{
	ShotgunLoadedAmmo = LoadedAmmo;
}

int AControllableSurvivor::GetPistolLoadedAmmo()
{
	return PistolLoadedAmmo;
}

void AControllableSurvivor::SetPistolLoadedAmmo(int LoadedAmmo)
{
	PistolLoadedAmmo = LoadedAmmo; 
}

void AControllableSurvivor::Reload() 
{
    // Don't reload if already reloading or if unarmed
    if (bIsReloading || UnarmedEquipped)
    {
        return;
    }

    bool needToReload = false;
    if (PistolEquipped)
    {
        // Check if pistol needs reload and has reserve ammo
        if (PistolLoadedAmmo < MAX_PISTOL_LOADED_AMMO && PistolReserveAmmo > 0)
        {
            needToReload = true;
        }
    }
    else if (ShotgunEquipped)
    {
        // Check if shotgun needs reload and has reserve ammo
        if (ShotgunLoadedAmmo < MAX_SHOTGUN_LOADED_AMMO && ShotgunReserveAmmo > 0)
        {
            needToReload = true;
        }
    }

    // Start reload process if needed
    if (needToReload)
    {
        ReloadPercentage = 0.0f;
        bIsReloading = true;
       
        // Play reloading sound depending on gun
        if (PistolEquipped && PistolReloadSound)
        {
            UGameplayStatics::PlaySound2D(this, PistolReloadSound, 0.7f, 1.0f, 0.0f);
        }
        else if (ShotgunEquipped && ShotgunReloadSound)
        {
            UGameplayStatics::PlaySound2D(this, ShotgunReloadSound, 0.7f, 1.0f, 0.0f);
        }
    }
}

float AControllableSurvivor::GetReloadStatus() 
{ 
	return ReloadPercentage / 100.0f; // standardize for Progress Bar UI component
}


// Weapon Management

bool AControllableSurvivor::GetUnarmedEquipped()
{
	return UnarmedEquipped;
}

void AControllableSurvivor::SetUnarmedEquipped(bool Equipped)
{
	UnarmedEquipped = Equipped;
}

bool AControllableSurvivor::GetPistolEquipped()
{
	return PistolEquipped;
}

void AControllableSurvivor::SetPistolEquipped(bool Equipped)
{
	PistolEquipped = Equipped;
}

bool AControllableSurvivor::GetShotgunEquipped()
{
	return ShotgunEquipped;
}

void AControllableSurvivor::SetShotgunEquipped(bool Equipped)
{
	ShotgunEquipped = Equipped; 
}

bool AControllableSurvivor::GetOwnsUnarmed() 
{ 
	return OwnsUnarmed; 
}

void AControllableSurvivor::SetOwnsUnarmed(bool Owned) 
{ 
	OwnsUnarmed = Owned; 
}

bool AControllableSurvivor::GetOwnsPistol() 
{ 
	return OwnsPistol; 
}

void AControllableSurvivor::SetOwnsPistol(bool Owned) 
{ 
	OwnsPistol = Owned; 
}

bool AControllableSurvivor::GetOwnsShotgun() 
{ 
	return OwnsShotgun; 
}

void AControllableSurvivor::SetOwnsShotgun(bool Owned) 
{ 
	OwnsShotgun = Owned; 
}


void AControllableSurvivor::SetScore(int NewScore)
{
	Score = NewScore;
}

void AControllableSurvivor::AddScore(int ScoreToAdd)
{
	Score += ScoreToAdd;
    UE_LOG(LogTemp, Warning, TEXT("Added score: %d"), ScoreToAdd);
}

void AControllableSurvivor::SetChickenCollected(int NewChickenCollected)
{
	ChickenCollected = NewChickenCollected;
}

void AControllableSurvivor::AddChickenCollected(int ChickenCollectedToAdd)
{
	ChickenCollected += ChickenCollectedToAdd;
    if (ChickenCollectedToAdd == 20)
    {
        UGameplayStatics::OpenLevel(this, FName("SuccessLevel"));
    }
}

const std::vector<AControllableSurvivor*>& AControllableSurvivor::GetSurvivorList()
{
  return Survivors;
}

void AControllableSurvivor::DealDamage(int32 Damage, FString DamageSource)
{
    Health -= Damage;
    std::string out = std::string(TCHAR_TO_UTF8(*DamageSource));
    if (DamageTakenSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DamageTakenSound, GetActorLocation());
    }
    UE_LOG(LogTemp, Warning, TEXT("Took %d damage from %s"), Damage, *FString(out.c_str()));
    if (Health <= 0)
    {
        UGameplayStatics::OpenLevel(this, FName("FailLevel"));
    }
}
