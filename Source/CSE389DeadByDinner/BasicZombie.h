// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

  bool CanAttackPlayer;

  // Player that has the aggression focus
  class AControllableSurvivor* AggroPlayer;

  // Last player to attack zombie
  class AControllableSurvivor* LastAttackedBy;

  float Health;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
