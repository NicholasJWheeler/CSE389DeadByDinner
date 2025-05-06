// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BasicProjectile.generated.h"

UCLASS()
class CSE389DEADBYDINNER_API ABasicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  UPROPERTY(VisibleDefaultsOnly)
  UStaticMeshComponent* MeshComp;

  UPROPERTY(VisibleAnywhere)
  UProjectileMovementComponent* PMComp;

  class AControllableSurvivor* Owner;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

  virtual int32 GetDamageDealt(class ABasicZombie* zombie);

  virtual void AddScoreFromZombie(int32 Score);

  virtual void Fire(FVector Direction, class AControllableSurvivor* owner);

  virtual void Init();

};
