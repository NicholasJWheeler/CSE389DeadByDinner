// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicZombie.h"

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

}

// Called to bind functionality to input
void ABasicZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

