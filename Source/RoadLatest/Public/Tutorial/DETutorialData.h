// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DETutorialData.generated.h"

UENUM(BlueprintType)
enum class ETutorialType : uint8
{
	Sequental,
	OnDemand
};


/**
 * 
 */
UCLASS(Blueprintable)
class ROADLATEST_API UDETutorialData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	TArray<FText> SequentialTutorials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	TArray<FText> OnDemandTutorials;
	

public:
	
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	bool IsValidTutorialID(ETutorialType InTutorialType, int32 InTutorialID) const;
};
