// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"

void UPlayerWidget::NativeConstruct()
{
    if (NameTextBlock)
    {
        NameTextBlock->SetText(FText::FromString("0 / 0"));
    }
}

void UPlayerWidget::updateAmmoCount(uint8 magAmmo, uint8 reserveAmmo)
{
    
    FString currentMag = FString::FromInt(magAmmo);
    FString currentReserve = FString::FromInt(reserveAmmo);

    NameTextBlock->SetText(FText::FromString(currentMag + " / " + currentReserve));

}
