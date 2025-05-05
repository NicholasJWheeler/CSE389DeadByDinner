// Fill out your copyright notice in the Description page of Project Settings.


#include "FadingFog.h"


// Sets default values
AFadingFog::AFadingFog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = true;

	FogSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FogSystem"));
	RootComponent = FogSystem;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FogAsset(TEXT("/Game/ParticleSystem/FX_Fog.FX_Fog"));
	if (FogAsset.Succeeded())
	{
		FogSystem->SetAsset(FogAsset.Object);
		FogSystem->SetAutoActivate(true);
	}
}

// Called when the game starts or when spawned
void AFadingFog::BeginPlay()
{
	Super::BeginPlay();

	CurrentOpacity = 0.0f;
	FogSystem->SetFloatParameter(OpacityParamName, CurrentOpacity);
	
}

// Called every frame
void AFadingFog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFadingIn)
	{
		CurrentOpacity += DeltaTime * FadeSpeed;
		if (CurrentOpacity >= 1.0f)
		{
			CurrentOpacity = 1.0f;
			bFadingIn = false; // switch to fading out
		}
	}
	else
	{
		CurrentOpacity -= DeltaTime * FadeSpeed;
		if (CurrentOpacity <= 0.0f)
		{
			CurrentOpacity = 0.0f;
			bFadingIn = true; // switch to fading in
		}
	}

	FogSystem->SetFloatParameter(OpacityParamName, CurrentOpacity);
}

