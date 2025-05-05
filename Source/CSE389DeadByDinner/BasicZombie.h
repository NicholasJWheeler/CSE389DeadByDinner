// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CSE389DeadByDinner/ZombieAIController.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "BasicZombie.generated.h"

UCLASS()
class CSE389DEADBYDINNER_API ABasicZombie : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicZombie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
  
	UCapsuleComponent* CollisionComp;

  UPROPERTY(EditAnywhere)
  UBoxComponent* PlayerAttackCollision;

  class AZombieAIController* AIController;
  
  void OnAIMoveCompleted(struct FAIRequestID,
                        const struct FPathFollowingResult& Result);

  // Player that has the aggression focus
  class AControllableSurvivor* AggroPlayer;

  // Last player to attack zombie
  class AControllableSurvivor* LastAttackedBy;

  UPROPERTY(EditAnywhere)
  float StoppingDistance = 50.0f;

  UFUNCTION()
  void MoveToPlayer();

  UFUNCTION()
  void SeekPlayer();

  UFUNCTION()
  void StopSeekingPlayer();

  int32 Health;
  int32 Speed;

  bool CanAttackPlayer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

  UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
