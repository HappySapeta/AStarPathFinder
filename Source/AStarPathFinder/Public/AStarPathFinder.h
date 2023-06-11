// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

// TODO : Come back to this if you think we should add something here.
struct FGraphNode
{
	TArray<FGraphNode*> AdjacentNodes;
};

struct FAStarGraphNode : FGraphNode
{
	FAStarGraphNode()
		:
	GCost(BIG_NUMBER),
	HCost(BIG_NUMBER),
	PathPenalty(0.0f),
	bIsWalkable(true),
	Parent(nullptr)
	{}
	
	explicit FAStarGraphNode(const FVector& WorldLocation)
		: FAStarGraphNode()
	{
		Location = WorldLocation;
	}

	bool operator==(const FAStarGraphNode& Other) const
	{
		return Location == Other.Location && AdjacentNodes == Other.AdjacentNodes;
	}

	// FCost getter.
	float FCost() const
	{
		// We don't really need extra storage for the FCost.
		return GCost + HCost;
	}

	// A clean node is a happy node.
	void Reset()
	{
		GCost = BIG_NUMBER;
		HCost = BIG_NUMBER;
		Parent = nullptr;
	}

	FVector Location = FVector::ZeroVector;

	float GCost, HCost, PathPenalty;
	bool bIsWalkable;
	FAStarGraphNode* Parent;
};

struct FMostOptimalNode
{
	bool operator()(const FAStarGraphNode& A, const FAStarGraphNode& B) const
	{
		return A.FCost() == B.FCost() ? A.HCost < B.HCost : A.FCost() < B.FCost();
	}
};

UENUM(BlueprintType)
enum class EHeuristic : uint8
{
	Euclidean = 0,
	Manhattan = 1,
	Chebyshev = 2,
	Octile = 3
};

/**
 * The A* Algorithm is an advanced BFS algorithm
 * that searches for shorter paths first rather than the longer paths.
 * It is optimal as well as a complete algorithm.
 */
class ASTARPATHFINDER_API FAStarPathFinder
{
public:
	
	// Initialise Heuristic Functions.
	FAStarPathFinder();
	
	// Find a path between Start and Destination node and output the result in an array of FAStarGraphNodes. 
	void FindPath(FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& OutPath);
	
	// Select a Heuristic Function
	void ChooseHeuristicFunction(const EHeuristic Choice);

protected:

	// Clear all pathfinding data and reset the graph to its original state.
	void ResetGraph();
	
private:

	// Join AStar Nodes by linking their parent nodes to form a chain that represents the path between Start and Destination node. 
	void CreatePath(const FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& Out_Path) const;

protected:

	// Mapping of Heuristic functions.
	TMap<EHeuristic, TFunction<float(const FVector&, const FVector&)>> HeuristicsMap;

	// The heuristic function being used to calculate distances between two points.
	TFunction<float(const FVector&, const FVector&)> HeuristicFunction;

	// An array of nodes that have not been visited yet.
	TArray<FAStarGraphNode*> Open;

	// A set of nodes that have already been visited.
	TSet<FAStarGraphNode*> Closed;
};
