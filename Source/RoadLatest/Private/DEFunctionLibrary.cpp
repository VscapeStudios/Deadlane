// Fill out your copyright notice in the Description page of Project Settings.


#include "DEFunctionLibrary.h"
#include "Components/PostProcessComponent.h"

void UDEFunctionLibrary::SortRacers(TArray<int32>& currentPlayers, TArray<float> distances)
{
	bool isSorted = false;
	int index = 0;
	int numPlayers = distances.Num();

	while (!isSorted)
	{
		index = 0;
		isSorted = true;

		while (index + 1 < numPlayers)
		{
			if (distances[index] < distances[index + 1])
			{
				distances.Swap(index, index + 1);
				currentPlayers.Swap(index, index + 1);
				isSorted = false;
			}

			index++;
		}
	}
}


