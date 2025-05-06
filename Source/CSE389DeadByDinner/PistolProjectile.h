// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BasicProjectile.h"
#include "PistolProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CSE389DEADBYDINNER_API APistolProjectile : public ABasicProjectile
{
	GENERATED_BODY()
	
public:

protected:

  UPROPERTY(VisibleDefaultsOnly)
  USphereComponent* CollisionComp;

  int32 BaseDamage = 25;

  float DamageFallOff = 0.08f / 100.0f;

  FVector InitialLocation;

  float DamageRadius = 15.0f;

public:

  virtual int32 GetDamageDealt(class ABasicZombie* Zombie) override;

  virtual void Fire(FVector Direction, class AControllableSurvivor* owner) override;

  virtual void Init() override;

};
