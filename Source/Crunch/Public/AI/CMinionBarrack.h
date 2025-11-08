// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "CMinionBarrack.generated.h"

UCLASS()
class CRUNCH_API ACMinionBarrack : public AActor
{
	GENERATED_BODY()

public:
	ACMinionBarrack();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category="Spawn")
	FGenericTeamId BarrackTeamID;

	UPROPERTY(EditAnywhere, Category="Spawn")
	int32 MinionPerGroup = 3;
	
	UPROPERTY(EditAnywhere, Category="Spawn")
	float GroupSpawnInterval = 5.f;

	UPROPERTY(EditAnywhere, Category="Spawn")
	AActor* MinionGoalActor;

	UPROPERTY()
	TArray<class ACMinion*> MinionPool;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<class ACMinion> MinionClass;

	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<class APlayerStart*> SpawnSpots;

	int32 NextSpawnSpotIndex = -1;

	APlayerStart* GetNextSpawnSpot();

	void SpawnNewGroup();
	void SpawnNewMinions(int32 Amount);
	ACMinion* GetNextAvailableMinion() const;

	FTimerHandle SpawnIntervalTimerHandle;
};
