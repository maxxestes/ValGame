// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM()
enum GunType
{
	Primary     UMETA(DisplayName = "Primary"),
	Secondary   UMETA(DisplayName = "Secondary"),
	Knife       UMETA(DisplayName = "Knife"),
};

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gun.generated.h"



UCLASS()
class VALGAME_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();





	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunProperties")
		float fireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunProperties")
		uint8 magSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunProperties")
		uint8 AutoOrSemi;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float WalkAimAccuracy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float maxRecoilMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float TimeToRevertToAccurate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float RunAimAccuracy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float InitialFiringAccuracy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float VerticleRecoilMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
		float HorizontalRecoilMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
		TEnumAsByte<GunType> type;

	UPROPERTY(EditAnywhere)
		USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ObjMesh;

	UPROPERTY(EditAnywhere)
		UBoxComponent* BoxCollider;



	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult);

	/** Attaches the actor to a FirstPersonCharacter */
	//UFUNCTION(BlueprintCallable, Category = "Gun")
	//void AttachWeapon(APlayerCharacter* TargetCharacter);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Show(bool visible);






public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:


	

};
