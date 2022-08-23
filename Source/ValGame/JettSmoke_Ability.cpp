// Fill out your copyright notice in the Description page of Project Settings.


#include "JettSmoke_Ability.h"

AJettSmoke_Ability::AJettSmoke_Ability() {
	this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	SetRootComponent(this->SceneComponent);

	this->SmokeBall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SmokeMesh"));
	this->SmokeBall->AttachTo(SceneComponent);

	this->Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));

	this->Hitbox->AttachTo(SmokeBall);

	NumberOfCharges = 2;

	isExpanded = false;

	SmokeLifetime = 5.0f;

	maximumTravelTime = 3.0f;

	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AJettSmoke_Ability::OnOverlapBegin);

}

void AJettSmoke_Ability::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(MaximumTravelHandle, this, &AJettSmoke_Ability::Expand, maximumTravelTime, false);
}




void AJettSmoke_Ability::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, "Wala");

	//if (OtherActor) {
		Expand();
//	}
		


}



void AJettSmoke_Ability::Expand() {
	if (GetWorld()->GetTimerManager().IsTimerActive(MaximumTravelHandle)) {
		GetWorld()->GetTimerManager().ClearTimer(MaximumTravelHandle);
	}
	SmokeBall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, "IWD");
	SmokeBall->SetSimulatePhysics(false);
	SmokeBall->SetEnableGravity(false);
	SmokeBall->SetWorldScale3D(FVector(7.0, 7.0, 7.0));
	isExpanded = true;
	GetWorld()->GetTimerManager().SetTimer(LifetimeExpandedHandle, this, &AJettSmoke_Ability::Kill, SmokeLifetime, false);
}



void AJettSmoke_Ability::Kill() {
	
	if (!this->Destroy()) {
		GEngine->AddOnScreenDebugMessage(1, 3, FColor::White, "No Kill");
	}
}
