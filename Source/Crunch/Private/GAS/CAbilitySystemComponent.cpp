// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

#include "GAS/CAttributeSet.h"


// Sets default values for this component's properties
UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UCAbilitySystemComponent::HealthUpdated);
}


// Called when the game starts
void UCAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCAbilitySystemComponent::AuthApplyGameplayEffect(const TSubclassOf<UGameplayEffect>& EffectToApply, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectToApply, Level, MakeEffectContext());
		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangedData)
{
	if (!GetOwner())
	{
		return;
	}

	if (ChangedData.NewValue <= 0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		AuthApplyGameplayEffect(DeathEffect);
	}
}


// Called every frame
void UCAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		AuthApplyGameplayEffect(EffectClass);
	}
}

void UCAbilitySystemComponent::GrantInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	for (const TPair<ECAbilityInputID, TSubclassOf<UCGameplayAbility>>& AbilityPair : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr);
		GiveAbility(AbilitySpec);
	}

	for (const TPair<ECAbilityInputID, TSubclassOf<UCGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr);
		GiveAbility(AbilitySpec);
	}
}

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(FullStatEffect);
}
