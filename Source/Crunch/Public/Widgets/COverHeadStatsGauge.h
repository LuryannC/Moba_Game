// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "COverHeadStatsGauge.generated.h"

class UCValueGauge;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCOverHeadStatsGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	void ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent) const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCValueGauge> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCValueGauge> ManaBar;
};
