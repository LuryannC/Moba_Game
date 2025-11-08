// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CMinionBarrack.h"

#include "AI/CMinion.h"
#include "GameFramework/PlayerStart.h"


// Sets default values
ACMinionBarrack::ACMinionBarrack()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACMinionBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &ACMinionBarrack::SpawnNewGroup, GroupSpawnInterval, true);
	}
}

void ACMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

APlayerStart* ACMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0)
	{
		return nullptr;
	}

	++NextSpawnSpotIndex;

	if (NextSpawnSpotIndex >= SpawnSpots.Num())
	{
		NextSpawnSpotIndex = 0;
	}

	return SpawnSpots[NextSpawnSpotIndex];
}

void ACMinionBarrack::SpawnNewGroup()
{
	int32 i = MinionPerGroup;
	while (i > 0)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}

		ACMinion* NextAvailableMinion = GetNextAvailableMinion();
		if (!NextAvailableMinion)
		{
			break;
		}

		NextAvailableMinion->SetActorTransform(SpawnTransform);
		NextAvailableMinion->Activate();
		--i;
	}

	SpawnNewMinions(i);
}

void ACMinionBarrack::SpawnNewMinions(int32 Amount)
{
	for (int32 i = 0; i < Amount; i++)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}

		ACMinion* NewMinion = GetWorld()->SpawnActorDeferred<ACMinion>(MinionClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		NewMinion->SetGenericTeamId(BarrackTeamID);
		NewMinion->FinishSpawning(SpawnTransform);
		NewMinion->SetGoal(MinionGoalActor);
		MinionPool.Add(NewMinion);
	}
}

ACMinion* ACMinionBarrack::GetNextAvailableMinion() const
{
	for (ACMinion* Minion : MinionPool)
	{
		if (!Minion->IsActive())
		{
			return Minion;
		}
	}

	return nullptr;;
}

