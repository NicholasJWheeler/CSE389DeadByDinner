// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectibleItem.h"
#include "ControllableSurvivor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACollectibleItem::ACollectibleItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Visual comp to hold mesh
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		VisualAsset(TEXT("/Game/Fab/Chicken_Nuggets/chicken_nuggets.chicken_nuggets"));

	if (VisualAsset.Succeeded()) {
		VisualMesh->SetStaticMesh(VisualAsset.Object);
		VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		VisualMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	}

	VisualMesh->SetGenerateOverlapEvents(true);
	VisualMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	VisualMesh->OnComponentBeginOverlap.AddDynamic(this, &ACollectibleItem::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ACollectibleItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollectibleItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator Rot = GetActorRotation();

	Rot.Yaw += 1.0f;
	SetActorRotation(Rot);

}

void ACollectibleItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AControllableSurvivor* Survivor = Cast<AControllableSurvivor>(OtherActor);
	if (Survivor)
	{
		if (Survivor->CollectSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Survivor->CollectSound, GetActorLocation());
		}
		Survivor->AddChickenCollected(1);
		Destroy();
	}
}


