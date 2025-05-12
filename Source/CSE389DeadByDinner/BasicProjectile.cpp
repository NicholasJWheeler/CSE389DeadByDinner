// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicProjectile.h"
#include "CSE389DeadByDinner/ControllableSurvivor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABasicProjectile::ABasicProjectile()
{
	// Set this actor to call Tick() every frame.	You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	CollisionComp->InitSphereRadius(DamageRadius);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RootComponent = CollisionComp;

	PMComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	PMComp->bRotationFollowsVelocity = true;
	PMComp->bShouldBounce = false;
	PMComp->ProjectileGravityScale = 0.f; // No gravity by default
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

int32 ABasicProjectile::GetDamageDealt(class ABasicZombie* zombie, class AControllableSurvivor* shooter)
{
    shooter = Owner;
	return 10;
}

void ABasicProjectile::AddScoreFromZombie(int Score)
{
	if (Owner)
	{
		Owner->AddScore(Score);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile has no Owner when trying to add score."));
	}
}

void ABasicProjectile::Fire(AControllableSurvivor *owner, FRotator Direction)
{
	Owner = owner;
	InitialLocation = GetActorLocation();
	PMComp->MaxSpeed = BulletSpeed;
	PMComp->InitialSpeed = BulletSpeed;
	PMComp->Velocity = GetActorForwardVector() * BulletSpeed;
	SetLifeSpan(Lifetime);

	if (RootComponent)
	{
		const int32 NumChildren = RootComponent->GetNumChildrenComponents();

		for (int32 i = 0; i < NumChildren; ++i)
		{
			USceneComponent *Child = RootComponent->GetChildComponent(i);
			if (UStaticMeshComponent *Mesh = Cast<UStaticMeshComponent>(Child))
			{
				Mesh->AddRelativeRotation(Direction);
			}
		}
	}
}
