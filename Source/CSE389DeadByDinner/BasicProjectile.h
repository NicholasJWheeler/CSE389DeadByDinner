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

	UPROPERTY(VisibleDefaultsOnly)
    USphereComponent* CollisionComp;
	
	UPROPERTY(EditAnywhere)
	int32 BaseDamage = 25;

	UPROPERTY(EditAnywhere)
    float DamageFallOff = 0.08f / 100.0f;
	
	UPROPERTY(EditAnywhere)
	FVector InitialLocation;

	UPROPERTY(EditAnywhere)
	float DamageRadius = 15.0f;

    UPROPERTY(EditAnywhere)
    float BulletSpeed = 100.0f;

	UPROPERTY(EditAnywhere)
    float Lifetime = 3.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetDamageDealt(class ABasicZombie *zombie, class AControllableSurvivor* shooter);

	void AddScoreFromZombie(int Score);

	void Fire(class AControllableSurvivor *owner, FRotator Direction);
};
