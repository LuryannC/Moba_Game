// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/Player/CPlayerController.h"
#include "Widgets/CGameplayWidget.h"
#include "Blueprint/UserWidget.h"
#include "Player/CPlayerCharacter.h"

void ACPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(InPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
	}
}

void ACPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);
	CPlayerCharacter = Cast<ACPlayerCharacter>(P);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalController())
	{
		return;
	}

	GameplayWidget = CreateWidget<UCGameplayWidget>(this, GameplayWidgetClass);
	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
	}
}
