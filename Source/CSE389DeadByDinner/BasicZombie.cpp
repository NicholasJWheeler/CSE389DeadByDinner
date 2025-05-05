// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicZombie.h"
#include "CSE389DeadByDinner/ControllableSurvivor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "ZombieAIController.h"
#include <algorithm>
#include <vector>

// Sets default values
ABasicZombie::ABasicZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
  
  Health = 100;
  CanAttackPlayer = true;

}

// Called when the game starts or when spawned
void ABasicZombie::BeginPlay() 
{
	Super::BeginPlay();

	// For controlling when something hits the character
	CollisionComp = FindComponentByClass<UCapsuleComponent>();

	if (CollisionComp)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &ABasicZombie::OnHit);
    PlayerAttackCollision = FindComponentByClass<UBoxComponent>();
    if (PlayerAttackCollision) {
      PlayerAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ABasicZombie::OnPlayerAttackOverlapBegin);
      PlayerAttackCollision->OnComponentEndOverlap.AddDynamic(this, &ABasicZombie::OnPlayerAttackOverlapEnd);
    } else {
      UE_LOG(LogTemp, Warning, TEXT("Could not add AttackCollision Dynamic Methods"));
    }
	}

  AIController = Cast<AZombieAIController>(GetController());
  AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ABasicZombie::OnAIMoveCompleted);
}

// Called every frame
void ABasicZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
  if (AggroPlayer == nullptr || !AggroPlayer) {
    // List of all survivors
    std::vector<AControllableSurvivor*> survivors = AControllableSurvivor::GetSurvivorList();
    if (survivors.size() > 0) {
      // Candidates that meet given requirements
      std::vector<AControllableSurvivor*> candidates;

      // Gathers candidate survivors
      std::for_each(survivors.begin(), survivors.end(), [&candidates](auto& x){
        if (x->GetHealth() > 0) {
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

void ABasicZombie::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Need to probably change this to get accurate player damage taken
	if (OtherComponent->GetCollisionProfileName().ToString() == "Projectile") {
		Health -= 10;
		UE_LOG(LogTemp, Warning, TEXT("Other component is enemy!"), Health);
	}

	// UE_LOG(LogTemp, Warning, TEXT("Health: %d"), Health);
}

void ABasicZombie::MoveToPlayer()
{
  if (AggroPlayer && AIController) {
    AIController->MoveToLocation(AggroPlayer->GetActorLocation(), StoppingDistance, true);
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
      &ABasicZombie::AttackPlayer, 1.0f, true);
  }
}

void ABasicZombie::OnPlayerAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp,
  class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex)
{
  GetWorld()->GetTimerManager().ClearTimer(AttackCooldown);
}

void ABasicZombie::AttackPlayer()
{
  // UE_LOG(LogTemp, Warning, TEXT("Attempting to attack survivor"));
  if (CanAttackPlayer && AggroPlayer) {
    AggroPlayer->DealDamage(10, TEXT("Basic Zombie"));
  }
}
