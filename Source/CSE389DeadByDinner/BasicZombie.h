// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CSE389DeadByDinner/ZombieAIController.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "BasicZombie.generated.h"

UCLASS()
class CSE389DEADBYDINNER_API ABasicZombie : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicZombie();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase *HitSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase *AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase *DeathSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
  
	UCapsuleComponent* CollisionComp;

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

  UPROPERTY(EditAnywhere)
  bool DisableAI;

  int32 Health;
  int32 Speed;

  UPROPERTY(EditAnywhere)
  UBoxComponent* PlayerAttackCollision;

  UFUNCTION()
  void OnPlayerAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp,
    class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
  void OnPlayerAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp,
    class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex);

  bool CanAttackPlayer;
  
  // For animation purposes
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool CurrentlyAttackingPlayer;

  FTimerHandle AttackCooldown;

  void AttackPlayer();

  void Die();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
						UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
						const FHitResult &SweepResult);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
