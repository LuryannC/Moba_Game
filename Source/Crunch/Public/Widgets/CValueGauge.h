// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CValueGauge.generated.h"

struct FOnAttributeChangeData;
struct FGameplayAttribute;
class UAbilitySystemComponent;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCValueGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	void SetAndBoundToGameplayAttributes(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& BaseAttribute, const FGameplayAttribute& MaxAttribute);
	void SetValue(float NewValue, float NewMaxValue);

private:
	
	void ValueChanged(const FOnAttributeChangeData& ChangedData);
	void MaxValueChanged(const FOnAttributeChangeData& ChangedData);

	float CachedValue;
	float CachedMaxValue;
	
	UPROPERTY(EditAnywhere, Category="Visual")
	FLinearColor BarColour;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	TObjectPtr<UTextBlock> ValueText;
};
