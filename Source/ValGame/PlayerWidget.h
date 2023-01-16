// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALGAME_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* TB_Ammo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* SageWallCounter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* JettSmokeCounter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* JettUpdraftCounter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* JettDashCounter;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* CurrentGun;


	void updateAmmoCount(uint8 magAmmo, uint8 reserveAmmo);

	void updateWallCount(uint8 AvailableWalls);

	void updateSmokeCount(uint8 AvailableSmokes);

	void updateUpdraftCount(uint8 AvailableUpdrafts);

	void updateDashCount(uint8 AvailableDashes);

	void updateGunDisplay(FString gunName);

	
};
