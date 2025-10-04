// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/COverHeadStatsGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/CAttributeSet.h"
#include "Widgets/CValueGauge.h"

void UCOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent) const
{
	AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (AbilitySystemComponent)
	{
		HealthBar.Get()->SetAndBoundToGameplayAttributes(AbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar.Get()->SetAndBoundToGameplayAttributes(AbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
	}
}
