// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableDoor.h"

// Sets default values
ABreakableDoor::ABreakableDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	isRaised = false;


	//Create our Default Components
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameMesh"));
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimelineComp"));
	DoorProxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorProximityVolume"));

	//Setup our Attachments
	DoorFrame->SetupAttachment(RootComponent);
	Door->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);
	DoorProxVolume->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void ABreakableDoor::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineEventStatic onTimelineFinishedCallback;

	//Binding our float track to our UpdateTimelineComp Function's output
	UpdateFunctionFloat.BindDynamic(this, &ABreakableDoor::UpdateTimelineComp);


	//If we have a float curve, bind it's graph to our update function
	if (DoorTimelineFloatCurve)
	{
		DoorTimelineComp->AddInterpFloat(DoorTimelineFloatCurve, UpdateFunctionFloat);
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("ChangeRaiseStatus") });
		DoorTimelineComp->SetTimelineFinishedFunc(onTimelineFinishedCallback);
	}
	
}

// Called every frame
void ABreakableDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABreakableDoor::ChangeRaiseStatus() {
	if (isRaised) {
		isRaised = false;
	}
	else {
		isRaised = true;
	}
}


void ABreakableDoor::RaiseDoor()
{
	if (!isRaised && !DoorTimelineComp->IsPlaying() && !DoorTimelineComp->IsReversing()) {
		DoorTimelineComp->Play();
	}
	else if (isRaised && !DoorTimelineComp->IsPlaying() && !DoorTimelineComp->IsReversing()) {
		DoorTimelineComp->Reverse();
	}
}

void ABreakableDoor::UpdateTimelineComp(float Output)
{


	FVector DoorNewLocation = FVector(0.0f, 45.f, Output);
	Door->SetRelativeLocation(DoorNewLocation);
}




