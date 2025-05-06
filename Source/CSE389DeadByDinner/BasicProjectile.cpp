// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicProjectile.h"
#include "CSE389DeadByDinner/ControllableSurvivor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABasicProjectile::ABasicProjectile()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));

  MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolProjectileMeshComponent"));
  static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Game/Assets/BlasterPack/bullet-foam-tip.bullet-foam-tip"));
  if (Mesh.Succeeded()) {
    MeshComp->SetStaticMesh(Mesh.Object);
    MeshComp->SetupAttachment(RootComponent);
  }
}

void ABasicProjectile::Init()
{
}

// Called when the game starts or when spawned
void ABasicProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ABasicProjectile::GetDamageDealt(class ABasicZombie* zombie)
{
  return 10;
}

void ABasicProjectile::AddScoreFromZombie(int32 Score)
{
  Owner->AddScore(Score);
}

void ABasicProjectile::Fire(FVector Direction, AControllableSurvivor* owner)
{
  Owner = owner;
}
