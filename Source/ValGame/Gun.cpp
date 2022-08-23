// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "PlayerCharacter.h"


// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	SetRootComponent(this->SceneComponent);





	this->ObjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));

	this->ObjMesh->AttachTo(SceneComponent);




	this->BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	this->BoxCollider->SetGenerateOverlapEvents(true);
	this->BoxCollider->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	this->BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AGun::OnOverlapBegin);
	this->BoxCollider->AttachToComponent(this->RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);


}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

}

void AGun::Show(bool visible)
{
	ECollisionEnabled::Type collision = visible ?
		ECollisionEnabled::QueryAndPhysics :
		ECollisionEnabled::NoCollision;

	this->SetActorTickEnabled(visible);

	this->ObjMesh->SetVisibility(visible);
	this->ObjMesh->SetCollisionEnabled(collision);

	this->BoxCollider->SetCollisionEnabled(collision);
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	FString pickup = FString::Printf(TEXT("Picked up: %s"), *GetName());
	GEngine->AddOnScreenDebugMessage(1, 5, FColor::White, pickup);


	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
		APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (player) {
			Show(false);

			player->PickUpGun(this);
		}
	}

	
}

void AGun::Reload(UPlayerWidget* widget)
{
	if (reserveAmmo != 0) {
		uint8 ammoNeeded = magSize - currentMagAmmo;
		if (reserveAmmo < ammoNeeded) {
			currentMagAmmo += reserveAmmo;
			reserveAmmo = 0;
		}
		else {
			currentMagAmmo = magSize;
			reserveAmmo -= ammoNeeded;
		}
		GEngine->AddOnScreenDebugMessage(1, 3, FColor::Red, "Reloading");
		widget->updateAmmoCount(currentMagAmmo, reserveAmmo);
		//Not working?
		//reserveAmmo < ammoNeeded ? currentMagAmmo += reserveAmmo, reserveAmmo = 0
		//	: currentMagAmmo = magSize, reserveAmmo -= ammoNeeded;
	}
}

uint8 AGun::ManageAmmoAfterShot(uint8 shotsFired)
{
	return currentMagAmmo -= shotsFired;
}

/*
void AGun::AttachWeapon(APlayerCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character != nullptr)
	{
		// Attach the weapon to the First Person Character
		//FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		//GetOwner()->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

		// Register so that Fire is called every time the character tries to use the item being held
		Character->OnUseItem.AddDynamic(this, &UTP_WeaponComponent::Fire);
	}
}
*/

