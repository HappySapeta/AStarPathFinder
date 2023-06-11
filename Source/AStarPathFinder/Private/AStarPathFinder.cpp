// Copyright Epic Games, Inc. All Rights Reserved.

#include "AStarPathFinder.h"
#include "..\Public\HeuristicLibrary.h"


FAStarPathFinder::FAStarPathFinder()
{
	// TODO : Maybe there's a better way to do this.
	HeuristicsMap.Add(EHeuristic::Euclidean, FHeuristicLibrary::GetEuclideanDistance);
	HeuristicsMap.Add(EHeuristic::Manhattan, FHeuristicLibrary::GetManhattanDistance);
	HeuristicsMap.Add(EHeuristic::Chebyshev, FHeuristicLibrary::GetChebyshevDistance);
	HeuristicsMap.Add(EHeuristic::Octile, FHeuristicLibrary::GetOctileDistance);
	
	HeuristicFunction = HeuristicsMap[EHeuristic::Manhattan];
}

void FAStarPathFinder::FindPath(FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& OutPath)
{
	if(!Start->bIsWalkable || !Destination->bIsWalkable)
	{
		return;
	}

	// Clean up things before making a mess again.
	ResetGraph();

	Start->GCost = 0.0f;
	Start->HCost = HeuristicFunction(Start->Location, Destination->Location);
	Start->Parent = nullptr; // Very important step, prevents a nast memory leak problem.
	
	Open.Push(Start);
	
	// While there are nodes left to be visited
	while (Open.Num() > 0)
	{
		// Current node is now marked as visited
		FAStarGraphNode* Current;
		Open.HeapPop(Current, FMostOptimalNode());
		Closed.Add(Current);
		
		// If we are already at destination, just break.
		if (Current == Destination)
		{
			break;
		}

		// For each neighboring node of the current node
		for (FGraphNode* GraphNode : Current->AdjacentNodes)
		{
			FAStarGraphNode* Neighbor = static_cast<FAStarGraphNode*>(GraphNode);
		
			// If the neighbor is already visited or not walkable (wall or obstacle), reject it and continue
			if (Closed.Contains(Neighbor) || !Neighbor->bIsWalkable)
			{
				continue;
			}
			
			// Calculate the cost of movement to this node.
			const float GCost = (Current->GCost + HeuristicFunction(Neighbor->Location, Current->Location)) * (1 + Neighbor->PathPenalty);
			
			// If the path to this node is cheaper, link the current node to this node as its parent.
			const bool bIsInOpen = Open.Contains(Neighbor);
			if (GCost < Neighbor->GCost || !bIsInOpen)
			{
				// Update path to Neighbor and its GCost, and HCost.
				Neighbor->GCost = GCost;
				Neighbor->HCost = HeuristicFunction(Neighbor->Location, Destination->Location);
				Neighbor->Parent = Current;
				
				// Neighbor's path has been updated but it hasn't been explored yet. Push it to Open so we can visit it later
				if (!bIsInOpen)
				{
					Open.HeapPush(Neighbor, FMostOptimalNode());
				}
			}
		}
	}

	// Final step : Form the path.
	CreatePath(Start, Destination, OutPath);
}

void FAStarPathFinder::ChooseHeuristicFunction(const EHeuristic Choice)
{
	if(!HeuristicsMap.Contains(Choice))
	{
		return;
	}
	
	HeuristicFunction = HeuristicsMap[Choice];
}

void FAStarPathFinder::CreatePath(const FAStarGraphNode* Start, const FAStarGraphNode* Destination, TArray<const FAStarGraphNode*>& Out_Path) const
{
	const FAStarGraphNode* Current = Destination;
	
	Out_Path.Reset();
	
	while(Current)
	{
		Out_Path.Add(Current);
		Current = Current->Parent;
	}

	Algo::Reverse(Out_Path);
}

void FAStarPathFinder::ResetGraph()
{
	Open.Reset();
	
	for (FAStarGraphNode* Node : Closed)
	{
		Node->Reset();
	}

	Closed.Reset();
}
