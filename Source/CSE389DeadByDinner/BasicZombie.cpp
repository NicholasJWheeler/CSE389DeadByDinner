// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicZombie.h"
#include "CSE389DeadByDinner/BasicProjectile.h"
#include "CSE389DeadByDinner/ControllableSurvivor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AITypes.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "ZombieAIController.h"
#include <algorithm>
#include <vector>

// Sets default values
ABasicZombie::ABasicZombie()
{
 	// Set this character to call Tick() every frame.	You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Health = 100;
	CanAttackPlayer = true;
	CurrentlyAttackingPlayer = false;

}

// Called when the game starts or when spawned
void ABasicZombie::BeginPlay() 
{
	Super::BeginPlay();

	// For controlling when something hits the character
	CollisionComp = FindComponentByClass<UCapsuleComponent>();

	if (CollisionComp)
	{
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABasicZombie::OnOverlapBegin);
		PlayerAttackCollision = FindComponentByClass<UBoxComponent>();
		if (PlayerAttackCollision) {
			PlayerAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ABasicZombie::OnPlayerAttackOverlapBegin);
			PlayerAttackCollision->OnComponentEndOverlap.AddDynamic(this, &ABasicZombie::OnPlayerAttackOverlapEnd);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Could not add AttackCollision Dynamic Methods"));
		}
	}

	AIController = Cast<AZombieAIController>(GetController());
    if (AIController)
		AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ABasicZombie::OnAIMoveCompleted);
}

// Called every frame
void ABasicZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	//float DistanceMoved = FVector::DistSquared(CurrentLocation, LastLocation);
	if (CurrentLocation.Z < 190.0f)
	{
		if (++StuckTicks > 10)
			{
                Unstuck();
                StuckTicks = 0;
			}
	}
	else {
            StuckTicks = 0;
	}

	LastLocation = CurrentLocation;

	if (AggroPlayer == nullptr || !AggroPlayer || !AggroPlayer->IsValidLowLevel()) {
            UE_LOG(LogTemp, Warning, TEXT("Searching for valid target"));
		// List of all survivors
		std::vector<AControllableSurvivor*> survivors = AControllableSurvivor::GetSurvivorList();
		if (survivors.size() > 0) {
			// Candidates that meet given requirements
			std::vector<AControllableSurvivor*> candidates;

			// Gathers candidate survivors
			std::for_each(survivors.begin(), survivors.end(), [&candidates](auto& x){
				if (x->GetHealth() > 0 && x->IsValidLowLevel()) {
					candidates.push_back(x);
				}
			});

			if (candidates.size() > 0) {
				// Sorts candidates by distance
				std::sort(candidates.begin(), candidates.end(), [this](auto& a, auto& b) -> bool {
					return (a->GetActorLocation() - this->GetActorLocation()).Size() > (b->GetActorLocation() - this->GetActorLocation()).Size();
				});

				// Puts aggro onto nearest candidate
				AggroPlayer = candidates[0];
			}
		}
	} else if (!DisableAI) {
		// Set aggro on last player to attack zombie
		if (LastAttackedBy) {
			AggroPlayer = LastAttackedBy;
		}

		if (this) {
			SeekPlayer();
		}
	} else {
		StopSeekingPlayer();
	}

}

// Called to bind functionality to input
void ABasicZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABasicZombie::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
												 int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	// Need to probably change this to get accurate player damage taken
		if (ABasicProjectile *proj = Cast<ABasicProjectile>(OtherActor))
		{
				int32 damage = proj->GetDamageDealt(this, LastAttackedBy);
                if (HitSound)
                {
                    UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
                }

				if ((Health - damage) <= 0)
				{
						proj->AddScoreFromZombie(1);
						Die();
				} else
				{
						proj->AddScoreFromZombie(0);
				}
				Health -= damage;
				proj->Destroy();
		}

	// UE_LOG(LogTemp, Warning, TEXT("Health: %d"), Health);
}

void ABasicZombie::MoveToPlayer()
{
    if (AggroPlayer && AggroPlayer->IsValidLowLevel() && AggroPlayer->GetHealth() > 0 && AIController)
    {
			try
			{
					AIController->MoveToLocation(AggroPlayer->GetActorLocation(), StoppingDistance, true);
			}
			catch (std::exception &e)
			{
					UE_LOG(LogTemp, Warning, TEXT("ERROR MOVING TO PLAYER: %s"), *FString(e.what()));
			}
	}
}

void ABasicZombie::SeekPlayer()
{
	MoveToPlayer();
}

void ABasicZombie::StopSeekingPlayer()
{
	AIController->StopMovement();
}

void ABasicZombie::OnAIMoveCompleted(struct FAIRequestID, const struct FPathFollowingResult& Result)
{
	StopSeekingPlayer();
}


void ABasicZombie::OnPlayerAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == AggroPlayer && !DisableAI) {
		// UE_LOG(LogTemp, Warning, TEXT("Attack Collision Overlapping with Survivor"));
		GetWorld()->GetTimerManager().SetTimer(AttackCooldown, this,
			&ABasicZombie::AttackPlayer, 1.0f, true, 0.5f);
	}
}

void ABasicZombie::OnPlayerAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	CurrentlyAttackingPlayer = false; // Do not continuously keep the attacking animation
	GetWorld()->GetTimerManager().ClearTimer(AttackCooldown);
}

void ABasicZombie::Unstuck() {
    UE_LOG(LogTemp, Warning, TEXT("Unstuck Zombie"));
    FVector CurrentLocation = GetActorLocation();
    FVector UnstuckLocation = CurrentLocation + FVector(0, 0, 50.0f); // move 50 units up

    SetActorLocation(UnstuckLocation, false, nullptr, ETeleportType::TeleportPhysics);

    // Optional: force falling mode in case they were stuck on something
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->SetMovementMode(MOVE_Falling);
    }

    UE_LOG(LogTemp, Warning, TEXT("Zombie was unstuck at: %s"), *UnstuckLocation.ToString());
}

void ABasicZombie::AttackPlayer()
{
	// UE_LOG(LogTemp, Warning, TEXT("Attempting to attack survivor"));
	if (CanAttackPlayer && AggroPlayer) {
		CurrentlyAttackingPlayer = true; // Start an attack animation
		AggroPlayer->DealDamage(10, TEXT("Basic Zombie"));
        if (AttackSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
        }

	}

	//CurrentlyAttackingPlayer = false; // Do not continuously keep the attacking animation
}

void ABasicZombie::Die()
{
		if (DeathSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
		}
		DisableAI = true;
		PrimaryActorTick.bCanEverTick = false;
		this->Destroy();
}
