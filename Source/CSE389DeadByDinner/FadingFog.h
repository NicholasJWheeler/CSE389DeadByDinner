// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FadingFog.generated.h"

UCLASS()
class CSE389DEADBYDINNER_API AFadingFog : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFadingFog();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* FogSystem;

	UPROPERTY(EditAnywhere, Category = "Fog Control")
	FName OpacityParamName = "FogOpacity";

	UPROPERTY(EditAnywhere, Category = "Fog Control")
	float FadeSpeed = 0.5f;

private:
	float CurrentOpacity = 0.0f;
	bool bFadingIn = true;

};
