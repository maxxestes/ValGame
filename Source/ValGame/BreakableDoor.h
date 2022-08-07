// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "BreakableDoor.generated.h"


UCLASS()
class VALGAME_API ABreakableDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableDoor();

	UPROPERTY(EditAnywhere)
		UCurveFloat* DoorTimelineFloatCurve;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float Health;

	//MeshComponents to represent Door assets
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* DoorFrame;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Door;

	

	//TimelineComponent to animate Door meshes
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTimelineComponent* DoorTimelineComp;

	//BoxComponent which will be used as our Proximity volume.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* DoorProxVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

	void RaiseDoor();

	float getHealth();

	void setHealth(float newHealth);

private:
	//Float Track Signature to handle our update track event
	FOnTimelineFloat UpdateFunctionFloat;

	//Function which updates our Door's relative location with the timeline graph
	UFUNCTION()
		void UpdateTimelineComp(float Output);

	UFUNCTION()
	void ChangeRaiseStatus();

	// Begin and End Overlap Events for our DoorProxVolume
	

	bool isRaised;

};
