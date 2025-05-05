// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"
#include "NavigationSystem.h"

void AZombieAIController::BeginPlay()
{
  Super::BeginPlay();

  NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);

}
