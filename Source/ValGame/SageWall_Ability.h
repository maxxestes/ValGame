// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "Components/TimelineComponent.h"
#include "SageWall_Ability.generated.h"

/**
 * 
 */
UCLASS()
class VALGAME_API ASageWall_Ability : public ACharacterAbility
{
	GENERATED_BODY()
	public:

		ASageWall_Ability();

		void PlaceWall();

		UPROPERTY(EditAnywhere)
			UCurveFloat* WallTimelineFloatCurve;

	protected:

		virtual void BeginPlay() override;


		//TimelineComponent to animate Door meshes
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
			UTimelineComponent* WallTimelineComp;

		//BoxComponent which will be used as our Proximity volume.
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			class UBoxComponent* WallProxVolume;



	private:

		//Float Track Signature to handle our update track event
		FOnTimelineFloat UpdateFunctionFloat;

		//Function which updates our Door's relative location with the timeline graph
		UFUNCTION()
			void UpdateTimelineComp(float Output);

		UPROPERTY(EditAnywhere)
		USceneComponent* SceneComponent;


		UPROPERTY(EditAnywhere)
		TArray<UStaticMeshComponent*> _wallArray;

	
};
