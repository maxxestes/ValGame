// Fill out your copyright notice in the Description page of Project Settings.


#include "SageWall_Ability.h"



ASageWall_Ability::ASageWall_Ability() {
	this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	SetRootComponent(this->SceneComponent);


	this->_wallArray.SetNum(5);

	this->rot = false;


	for (int i = 0; i < 5; i++) {
		//USageWallComp newSegment = USageWallComp();
		this->_wallArray[i] = CreateDefaultSubobject<USageWallComp>(TEXT("WallMesh" + i));
		this->_wallArray[i]->AttachTo(SceneComponent);
	}
	
	WallTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimelineComp"));
}


void ASageWall_Ability::PlaceWall() 
{
	
	WallTimelineComp->Play();
	for (int i = 0; i < 5; i++) {	
		this->_wallArray[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ASageWall_Ability::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineEventStatic onTimelineFinishedCallback;

	//Binding our float track to our UpdateTimelineComp Function's output
	UpdateFunctionFloat.BindDynamic(this, &ASageWall_Ability::UpdateTimelineComp);


	//If we have a float curve, bind it's graph to our update function
	if (WallTimelineFloatCurve)
	{
		WallTimelineComp->AddInterpFloat(WallTimelineFloatCurve, UpdateFunctionFloat);
		//onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("ChangeRaiseStatus") });
		//WallTimelineFloatCurve->SetTimelineFinishedFunc(onTimelineFinishedCallback);
	}
}

void ASageWall_Ability::UpdateTimelineComp(float Output)
{
	FVector prevScale = this->SceneComponent->GetRelativeScale3D();
	prevScale.Z = Output;
	this->SceneComponent->SetRelativeScale3D(prevScale);
}



