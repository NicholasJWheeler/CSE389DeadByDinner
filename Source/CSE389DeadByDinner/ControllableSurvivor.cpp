// Fill out your copyright notice in the Description page of Project Settings.


#include "ControllableSurvivor.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Math/Plane.h" 
#include "Math/UnrealMathUtility.h" // for cursor position rotation
#include "EnhancedInputComponent.h"

// Sets default values
AControllableSurvivor::AControllableSurvivor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Variables
	Score = 0; // Start with no points
	ChickenCollected = 0; // Start with no food collected
	Health = 100; // Start with full health
	PistolLoadedAmmo = 0; // Start with no loaded pistol ammo
	PistolReserveAmmo = 0; // Start with no reserve pistol ammo
	ShotgunLoadedAmmo = 0; // Start with no loaded shotgun ammo
	ShotgunReserveAmmo = 0; // Start with no reserve shotgun ammo

	// Starting weapon is nothing
	UnarmedEquipped = false;
	PistolEquipped = true;
	ShotgunEquipped = false;
	

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
	PEI->BindAction(InputShoot, ETriggerEvent::Triggered, this, &AControllableSurvivor::Shoot);

}

void AControllableSurvivor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Need to probably change this to get accurate player damage taken
	if (OtherComponent->GetCollisionProfileName().ToString() == "Enemy") {
		Health -= 10;
		UE_LOG(LogTemp, Warning, TEXT("Other component is enemy!"), Health);
	}

	UE_LOG(LogTemp, Warning, TEXT("Health: %d"), Health);
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
	// If no gun equipped, do nothing
	// If pistol equipped, shoot 1 bullet per click, spawn small explosion niagra effect at gun barrel
	// If shotgun equipped, shoot 5 bullets in a short range star pattern, spawn cone niagra explosion effect at gun barrel






	// Set ProjectileSpawn to spawn projectiles slightly in front of the character
	//ProjectileSpawn.Set(100.0f, 0.0f, 0.0f);

	//// Transform ProjectileSpwan from local space to world space.
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
	Health -= HealthToSubtract;
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


void AControllableSurvivor::SetScore(int NewScore)
{
	Score = NewScore;
}

void AControllableSurvivor::AddScore(int ScoreToAdd)
{
	Score += ScoreToAdd;
}

void AControllableSurvivor::SetChickenCollected(int NewChickenCollected)
{
	ChickenCollected = NewChickenCollected;
}

void AControllableSurvivor::AddChickenCollected(int ChickenCollectedToAdd)
{
	ChickenCollected += ChickenCollectedToAdd;
}


