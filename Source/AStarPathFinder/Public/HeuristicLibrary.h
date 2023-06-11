// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * The HeuristicLibrary is a collection of mathematical functions
 * that calculate the distance between two points.
 */
class ASTARPATHFINDER_API FHeuristicLibrary
{
public:
	
	// Manhattan distance is calculated as the sum of the absolute differences between the two vectors.
	static float GetManhattanDistance(const FVector& V1,const FVector& V2)
	{
		return abs(V1.X - V2.X) + abs(V1.Y - V2.Y);
	}

	// Euclidean distance between two points in Euclidean space
	// is the length of a line segment between the two points. 
	static float GetEuclideanDistance(const FVector& V1,const FVector& V2)
	{
		return FVector::Distance(V1, V2);
	}

	// Chebyshev distance, is a metric defined on a vector space
	// where the distance between two vectors is the greatest of their differences along any coordinate dimension. 
	static float GetChebyshevDistance(const FVector& V1,const FVector& V2)
	{
		return FMath::Max(abs(V1.X - V2.X), abs(V1.Y - V2.Y));
	}

	// The Octile distance is used to estimate the distance between two cells heuristically.
	// The respective lengths of cardinal and diagonal moves are 1 and 1.414.
	// A matrix the same size as the map is used to store all the grid points.
	static float GetOctileDistance(const FVector& V1,const FVector& V2)
	{
		const float DelX = abs(V1.X - V2.X);
		const float DelY = abs(V1.Y - V2.Y);
		return (DelX + DelY) * (UE_SQRT_2 - 2) * FMath::Min(DelX, DelY);
	}
};
