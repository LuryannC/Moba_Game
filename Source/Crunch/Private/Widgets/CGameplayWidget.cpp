// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CGameplayWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/CAttributeSet.h"
#include "Widgets/CValueGauge.h"

void UCGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		HealthBar.Get()->SetAndBoundToGameplayAttributes(OwnerAbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar.Get()->SetAndBoundToGameplayAttributes(OwnerAbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
	}
}
