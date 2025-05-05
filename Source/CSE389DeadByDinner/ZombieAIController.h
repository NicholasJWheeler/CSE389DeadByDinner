// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class CSE389DEADBYDINNER_API AZombieAIController : public AAIController
{
	GENERATED_BODY()
	
public:
  void BeginPlay() override;

private:

  class UNavigationSystemV1* NavArea;

public:

};
