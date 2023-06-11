// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AStarPathFinder.h"
#include "AStarNavVolume.generated.h"

class UBoxComponent;
/**
 * A simple implementation of a navigation volume that
 * spawns collision detection boxes in a user specified
 * region, in a grid like arrangement, to map walkable places.
 *
 * This class also serves as a good example of how the FAStarPathFinder class is used.
 */
UCLASS()
class ASTARNAVVOLUME_API AAStarNavVolume : public AActor
{
	GENERATED_BODY()

public:

	AAStarNavVolume();
	
	// Use AStar Pathfinding to find the shortest path between Start and Destination locations if they are valid walkable locations.
	UFUNCTION(BlueprintCallable)
	void FindPath(const FVector& Start, const FVector& Destination);

	// Select a heuristic function that defines the distance between two points.
	UFUNCTION(BlueprintCallable)
	void ChooseHeuristicFunction(const EHeuristic Choice);

	// Check if a World Location is associated to valid node in the grid.
	UFUNCTION(BlueprintCallable)
	bool CheckWorldLocation(const FVector& WorldLocation) const;
	
protected:

	// Calls GenerateGrid().
	virtual void BeginPlay() override;
	
private:

	// Spawn collision detection blocks and map blocked and walkable areas.
	virtual void GenerateGraph();

	// Checks if give row, column pair is a valid location in the grid.
	bool IsValidGridLocation(uint32 Row, uint32 Column) const;

	// Map a World Location to a location in the grid.
	bool GetGraphNodeFromLocation(const FVector& WorldLocation, int32& Row, int32& Column) const;

	// Link AStar nodes to their neighbors.
	void CreateLinks();

	// Link One node to a specific neighbor identified by its row and column values.
	void LinkNodes(FAStarGraphNode& TargetNode, uint32 FromRow, uint32 FromColumn);

private:

	UPROPERTY(EditAnywhere, Category = "Nav Volume")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Nav Volume")
	float AgentSize = 100.0f;
	
	// A two-dimensional array containing all Block Actors (Collision detection boxes)
	TArray<TArray<FAStarGraphNode>> NavGraph;

	// Bounds of the map.
	FVector MinWorldLocation, MaxWorldLocation;

	// AStar Pathfinder object.
	FAStarPathFinder PathFinder;
	
};
