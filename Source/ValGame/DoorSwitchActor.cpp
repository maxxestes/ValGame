// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorSwitchActor.h"
#include "PlayerCharacter.h"

// Sets default values
ADoorSwitchActor::ADoorSwitchActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	SetRootComponent(this->SceneComponent);




	this->ObjMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));

	this->ObjMesh->AttachTo(SceneComponent);




	this->BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	this->BoxCollider->SetGenerateOverlapEvents(true);
	this->BoxCollider->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	this->BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ADoorSwitchActor::OnOverlapBegin);
	this->BoxCollider->AttachToComponent(this->RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

}

// Called when the game starts or when spawned
void ADoorSwitchActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoorSwitchActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	FString doorSwitch = FString::Printf(TEXT("In range of: %s"), *GetName());
	GEngine->AddOnScreenDebugMessage(1, 5, FColor::White, doorSwitch);


	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
		APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (player) {

			player->SwitchProximityEnter(this);
		}
	}

	GEngine->AddOnScreenDebugMessage(1, 3, FColor::Red, *doorSwitch);
}

void ADoorSwitchActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	FString doorSwitch = FString::Printf(TEXT("In range of: %s"), *GetName());
	GEngine->AddOnScreenDebugMessage(1, 5, FColor::White, doorSwitch);


	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) {
		APlayerCharacter* player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (player) {

			player->SwitchProximityExit();
		}
	}

	GEngine->AddOnScreenDebugMessage(1, 3, FColor::Red, *doorSwitch);
}



// Called every frame
void ADoorSwitchActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

