// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DEFunctionLibrary.generated.h"

/**
 * 
 */
 class UPostProcessComponent;

UCLASS()
class ROADLATEST_API UDEFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
		UFUNCTION(BlueprintCallable)
		static void SortRacers(UPARAM(ref) TArray<int32>& currentPlayers, TArray<float> distances);

};
