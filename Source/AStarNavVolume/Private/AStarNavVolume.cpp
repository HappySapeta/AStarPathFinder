// Copyright Epic Games, Inc. All Rights Reserved.

#include "AStarNavVolume.h"
#include "AStarNavVolumeModifier.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void AAStarNavVolume::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateGraph();
}

void AAStarNavVolume::GenerateGraph()
{
	const float NavDimensionX = BoxComponent->GetScaledBoxExtent().X;
	const float NavDimensionY = BoxComponent->GetScaledBoxExtent().Y;
	const FVector& NavVolumeOrigin = GetActorLocation();
	
	MaxWorldLocation = NavVolumeOrigin + GetActorRightVector() * NavDimensionY + GetActorForwardVector() * NavDimensionX;
	MinWorldLocation = NavVolumeOrigin - GetActorRightVector() * NavDimensionY - GetActorForwardVector() * NavDimensionX;

	const uint32 NumRows = (NavDimensionX / AgentSize) * 2;
	const uint32 NumCols = (NavDimensionY / AgentSize) * 2;

	const UWorld* World = GetWorld();

	FVector Location = MinWorldLocation;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	
	for (uint32 Row = 0; Row < NumRows; ++Row)
	{
		NavGraph.Add(TArray<FAStarGraphNode>());
		for (uint32 Column = 0; Column < NumCols; ++Column)
		{
			NavGraph[Row].Add(FAStarGraphNode(Location + (GetActorForwardVector() * AgentSize / 2) + (GetActorRightVector() * AgentSize / 2)));
			FAStarGraphNode& NewNode = NavGraph[Row][Column];
			
			TArray<AActor*> HitActors;
			if(UKismetSystemLibrary::BoxOverlapActors(World, Location, FVector(AgentSize), ObjectTypes, AActor::StaticClass(), {this}, HitActors))
			{
				AAStarNavVolumeModifier* Modifier;
				if(HitActors.FindItemByClass<AAStarNavVolumeModifier>(&Modifier))
				{
					NewNode.PathPenalty = Modifier->GetPenalty();
					NewNode.bIsWalkable = Modifier->GetWalkability();
				}
			}
			
			Location += GetActorRightVector() * AgentSize;
		}

		Location += GetActorForwardVector() * AgentSize;
		Location.Y = MinWorldLocation.Y;
	}

	CreateLinks();
}

AAStarNavVolume::AAStarNavVolume()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
}

void AAStarNavVolume::FindPath(const FVector& Start, const FVector& Destination)
{
	FAStarGraphNode* StartNode = nullptr;
	const FAStarGraphNode* DestinationNode = nullptr;
	
	int32 Row, Column;
	if(GetGraphNodeFromLocation(Start, Row, Column))
	{
		StartNode = &NavGraph[Row][Column];
	}
	
	if(GetGraphNodeFromLocation(Destination, Row, Column))
	{
		DestinationNode = &NavGraph[Row][Column];
	}

	if(!StartNode || !DestinationNode)
	{
		return;
	}

	TArray<const FAStarGraphNode*> Path;
	PathFinder.FindPath(StartNode, DestinationNode, Path);

	const UWorld* World = GetWorld();
	UKismetSystemLibrary::FlushPersistentDebugLines(World);

	for(int32 Index = 0; Index < Path.Num() - 1; ++Index)
	{
		DrawDebugLine(World, Path[Index]->Location, Path[Index + 1]->Location, FColor::Blue, true, -1, 0, 20.0f);
	}
}

void AAStarNavVolume::ChooseHeuristicFunction(const EHeuristic Choice)
{
	PathFinder.ChooseHeuristicFunction(Choice);
}

void AAStarNavVolume::CreateLinks()
{
	for(int32 Row = 0; Row < NavGraph.Num(); ++Row)
	{
		for(int32 Column = 0; Column < NavGraph[Row].Num(); ++Column)
		{
			FAStarGraphNode& Node = NavGraph[Row][Column];
			LinkNodes(Node, Row, Column + 1);
			LinkNodes(Node, Row + 1, Column);
			LinkNodes(Node, Row, Column - 1);
			LinkNodes(Node, Row - 1, Column);
			LinkNodes(Node, Row - 1, Column + 1);
			LinkNodes(Node, Row + 1, Column + 1);
			LinkNodes(Node, Row + 1, Column - 1);
			LinkNodes(Node, Row - 1, Column - 1);
		}
	}
}

void AAStarNavVolume::LinkNodes(FAStarGraphNode& TargetNode, const uint32 FromRow, const uint32 FromColumn)
{
	if(IsValidGridLocation(FromRow, FromColumn))
	{
		TargetNode.AdjacentNodes.Add(&NavGraph[FromRow][FromColumn]);
	}
}

bool AAStarNavVolume::IsValidGridLocation(const uint32 Row, const uint32 Column) const
{
	return NavGraph.IsValidIndex(Row) && NavGraph[Row].IsValidIndex(Column);
}

bool AAStarNavVolume::CheckWorldLocation(const FVector& WorldLocation) const
{
	return WorldLocation.X >= MinWorldLocation.X && WorldLocation.Y >= MinWorldLocation.Y
		&& WorldLocation.X <= MaxWorldLocation.X && WorldLocation.Y <= MaxWorldLocation.Y;
}

bool AAStarNavVolume::GetGraphNodeFromLocation(const FVector& WorldLocation, int32& Row, int32& Column) const
{
	Row = -1;
	Column = -1;

	if (!CheckWorldLocation(WorldLocation))
	{
		return false;
	}

	const FVector& RelativeWorldLocation = WorldLocation - MinWorldLocation;
	
	Row = (RelativeWorldLocation.X / AgentSize);
	Column = (RelativeWorldLocation.Y / AgentSize);

	return true;
}

