// Fill out your copyright notice in the Description page of Project Settings.


#include "SageWallComponent.h"

// Sets default values for this component's properties
USageWallComponent::USageWallComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	//SetRootComponent(this->SceneComponent);

	//static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube"));
}


// Called when the game starts
void USageWallComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USageWallComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

