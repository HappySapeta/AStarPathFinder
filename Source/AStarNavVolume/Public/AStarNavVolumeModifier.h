// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStarNavVolumeModifier.generated.h"

UCLASS()
class ASTARNAVVOLUME_API AAStarNavVolumeModifier : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAStarNavVolumeModifier()
	{
		PrimaryActorTick.bCanEverTick = false;
	}

	bool GetWalkability() const
	{
		return bIsWalkable;
	}
	
	float GetPenalty() const
	{
		return Penalty;
	}

private:

	UPROPERTY(EditAnywhere, Category = "Modifier Settings", meta = (ClampMin = 0.0, UIMin = 0.0, ClampMax = 1.0, UIMax = 1.0))
	float Penalty = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Modifier Settings")
	bool bIsWalkable = true;
	
};
