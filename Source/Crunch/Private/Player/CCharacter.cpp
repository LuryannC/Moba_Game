// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Player/CCharacter.h"

#include "Components/WidgetComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/COverHeadStatsGauge.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("CAbilitySystemComponent"));
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("CAttributeSet"));
	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadStatsGauge"));
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadWidget();
}

void ACCharacter::ServerSideInit()
{
	CAbilitySystemComponent.Get()->InitAbilityActorInfo(this, this);
	CAbilitySystemComponent.Get()->ApplyInitialEffects();
}

void ACCharacter::ClientSideInit()
{
	CAbilitySystemComponent.Get()->InitAbilityActorInfo(this, this);
}

void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return CAbilitySystemComponent;
}

bool ACCharacter::IsLocallyControlledByPlayer() const
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
}

void ACCharacter::ConfigureOverHeadWidget()
{
	if (!OverHeadWidgetComponent)
	{
		return;
	}

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	UCOverHeadStatsGauge* OverHeadStatsGauge = Cast<UCOverHeadStatsGauge>(OverHeadWidgetComponent.Get()->GetUserWidgetObject());
	if (OverHeadStatsGauge)
	{
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
		OverHeadWidgetComponent->SetHiddenInGame(false);

		GetWorldTimerManager().ClearTimer(StatGaugeVisibilityUpdateTimerHandle);
		GetWorldTimerManager().SetTimer(
			StatGaugeVisibilityUpdateTimerHandle,
			this,
			&ACCharacter::UpdateStatGaugeVisibility,
			HeadStatGaugeVisibilityUpdateRate,
			true);
	}
}

void ACCharacter::UpdateStatGaugeVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		float DistanceSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistanceSquared > HeadStatGaugeVisibilityRangeSquared);
	}
}
