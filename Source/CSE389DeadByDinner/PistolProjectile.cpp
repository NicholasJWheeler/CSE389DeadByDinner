// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolProjectile.h"
#include "CSE389DeadByDinner/BasicZombie.h"
#include "Components/SphereComponent.h"
#include "CSE389DeadByDinner/ControllableSurvivor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

void APistolProjectile::Init()
{
  CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

  CollisionComp->InitSphereRadius(DamageRadius);

  RootComponent = CollisionComp;

  MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolProjectileMeshComponent"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/Assets/BlasterPack/bullet-foam-tip.bullet-foam-tip"));
  if (Mesh.Succeeded()) {
    MeshComp->SetStaticMesh(Mesh.Object);
    MeshComp->SetupAttachment(RootComponent);
  }

  InitialLocation = Owner->GetActorLocation();
}

int32 APistolProjectile::GetDamageDealt(class ABasicZombie* Zombie)
{
  float Dist = (Zombie->GetActorLocation() - InitialLocation).Size();
  int32 DamageRemoved = DamageFallOff * Dist * BaseDamage;
  return (DamageRemoved >= BaseDamage) ?  1 : BaseDamage - DamageRemoved + 1;
}

void APistolProjectile::Fire(FVector Direction, class AControllableSurvivor* owner)
{
  Super::Fire(Direction, owner);
  Init();
}
