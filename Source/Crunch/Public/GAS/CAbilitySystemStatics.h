// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define NET_LOG(ContextObject, CategoryName, Verbosity, Format, ...) \
	{ \
		FString Prefix = GetNetPrefix(ContextObject); \
		FString Msg = FString::Printf(TEXT("%s") Format, *Prefix, ##__VA_ARGS__); \
		UE_LOG(CategoryName, Verbosity, TEXT("%s"), *Msg); \
	}

inline FString GetNetPrefix(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return TEXT("[NoContext] ");
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return TEXT("[NoWorld] ");
	}

	ENetMode NetMode = World->GetNetMode();
	switch (NetMode)
	{
	case NM_Standalone:
		return TEXT("[Standalone] ");

	case NM_DedicatedServer:
	case NM_ListenServer:
		return TEXT("[Server] ");

	case NM_Client:
		{
			int32 ClientId = 0;

#if WITH_EDITOR
			// Each PIE instance gets a unique ID
			if (const FWorldContext* Context = GEngine->GetWorldContextFromWorld(World))
			{
				ClientId = Context->PIEInstance;
			}
#else
			// In packaged builds, use player controller index
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (APlayerState* PS = PC->PlayerState)
				{
					ClientId = PS->GetPlayerId();
				}
			}
#endif

			return FString::Printf(TEXT("[Client %d] "), ClientId);
		}

	default:
		return TEXT("[Unknown] ");
	}
}