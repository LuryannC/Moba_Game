// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CGameplayWidget.generated.h"

class UAbilitySystemComponent;
class UCValueGauge;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCGameplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCValueGauge> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCValueGauge> ManaBar;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
};
