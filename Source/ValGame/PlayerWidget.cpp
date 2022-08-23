// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"

void UPlayerWidget::NativeConstruct()
{
    if (TB_Ammo)
    {
        TB_Ammo->SetText(FText::FromString("0 / 0"));
    }

    if (SageWallCounter)
    {
        SageWallCounter->SetText(FText::FromString("Wall Count: 1"));
    }

    if (JettSmokeCounter)
    {
        JettSmokeCounter->SetText(FText::FromString("Smoke Count: 2"));
    }

    if (CurrentGun)
    {
        CurrentGun->SetText(FText::FromString("NO WEAPON"));
    }
}

void UPlayerWidget::updateAmmoCount(uint8 magAmmo, uint8 reserveAmmo)
{
    
    FString currentMag = FString::FromInt(magAmmo);
    FString currentReserve = FString::FromInt(reserveAmmo);

    TB_Ammo->SetText(FText::FromString(currentMag + " / " + currentReserve));

}

void UPlayerWidget::updateWallCount(uint8 AvailableWalls)
{
    FString currentWalls = FString::FromInt(AvailableWalls);
    SageWallCounter->SetText(FText::FromString("Wall Count: " + currentWalls));
}

void UPlayerWidget::updateSmokeCount(uint8 AvailableSmokes)
{
    FString currentSmokes = FString::FromInt(AvailableSmokes);
    JettSmokeCounter->SetText(FText::FromString("Smoke Count: " + currentSmokes));
}

void UPlayerWidget::updateGunDisplay(FString gunName)
{
    CurrentGun->SetText(FText::FromString("Current Gun: " + gunName));
}
