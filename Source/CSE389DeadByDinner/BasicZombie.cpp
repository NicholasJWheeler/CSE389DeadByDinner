// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicZombie.h"
#include "CSE389DeadByDinner/ControllableSurvivor.h"
#include "GameFramework/Actor.h"
#include <algorithm>
#include <vector>

// Sets default values
ABasicZombie::ABasicZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABasicZombie::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
  if (AggroPlayer == nullptr) {
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
  } else {
    // Set aggro on last player to attack zombie
    if (LastAttackedBy != nullptr) {
      AggroPlayer = LastAttackedBy;
    }
  }

}

// Called to bind functionality to input
void ABasicZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

