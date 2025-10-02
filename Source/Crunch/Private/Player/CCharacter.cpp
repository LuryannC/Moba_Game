// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Player/CCharacter.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("CAbilitySystemComponent"));
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("CAttributeSet"));
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
