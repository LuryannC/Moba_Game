// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CValueGauge.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();

	ProgressBar.Get()->SetFillColorAndOpacity(BarColour);
}

void UCValueGauge::SetAndBoundToGameplayAttributes(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& BaseAttribute,
	const FGameplayAttribute& MaxAttribute)
{
	if (AbilitySystemComponent)
	{
		bool bFound;
		const float Value = AbilitySystemComponent->GetGameplayAttributeValue(BaseAttribute, bFound);
		const float MaxValue = AbilitySystemComponent->GetGameplayAttributeValue(MaxAttribute, bFound);
		if (bFound)
		{
			SetValue(Value, MaxValue);
		}

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttribute).AddUObject(this, &UCValueGauge::ValueChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UCValueGauge::MaxValueChanged);
	}
}


void UCValueGauge::SetValue(float NewValue, float NewMaxValue)
{
	CachedValue = NewValue;
	CachedMaxValue = NewMaxValue;
	
	if (NewMaxValue == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCValueGauge::SetValue - Value Gauge NewMaxValue can't be 0."));
		return;
	}
	
	const float NewPercentage = NewValue / NewMaxValue;
	ProgressBar.Get()->SetPercent(NewPercentage);

	const FNumberFormattingOptions FormattingOptions = FNumberFormattingOptions().SetMaximumFractionalDigits(0);
	ValueText.Get()->SetText(FText::Format(
		FTextFormat::FromString("{0}/{1}"),
		FText::AsNumber(NewValue, &FormattingOptions),
		FText::AsNumber(NewMaxValue, &FormattingOptions)
		)
	);
}

void UCValueGauge::ValueChanged(const FOnAttributeChangeData& ChangedData)
{
	SetValue(ChangedData.NewValue, CachedMaxValue);
}

void UCValueGauge::MaxValueChanged(const FOnAttributeChangeData& ChangedData)
{
	SetValue(CachedValue, ChangedData.NewValue);
}
