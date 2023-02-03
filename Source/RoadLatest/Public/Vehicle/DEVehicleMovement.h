// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "DEVehicleMovement.generated.h"

/**
 * 
 */
UCLASS()
class ROADLATEST_API UDEVehicleMovement : public UWheeledVehicleMovementComponent4W
{
	GENERATED_BODY()

	UDEVehicleMovement(const FObjectInitializer& ObjectInitializer);

protected:
	

	bool IsWheelDrive(const FWheelSetup& WheelSetup);

	float GetCurrentMaxRPM() const;

	void SetMaxRPM(float NewMaxRPM);



public:


	/*Return current Torque factor. */
	UFUNCTION(BlueprintCallable, Category = "Vehicle Movement")
	float GetTorqueFactor() const;


	/* Set Torque multiplier will be applied to all points in torque curve*/
	UFUNCTION(BlueprintCallable, Category = "Vehicle Movement")
	void SetTorqueFactor(float NewTorqueFactor= 1.0f);


	/*Get maximum speed of the kart can reach*/
	UFUNCTION(BlueprintCallable, Category = "Vehicle Movement")
	float GetVehicleMaxSpeed();


	/*Get maximum speed of the kart be default*/
	UFUNCTION(BlueprintCallable, Category = "Vehicle Movement")
	float GetDefaultVehicleMaxSpeed();

	/*Modify max engine RPM to limit max speed */
	UFUNCTION(BlueprintCallable, Category = "Vehicle Movement")
	void SetVehicleMaxSpeed(float NewMaxSpeed);

	/*Reset max speed of the vehicle to defaults*/
	UFUNCTION(BlueprintCallable, Category = "Vehicle Movement")
	void ResetVehicleMaxSpeed();



private:


	void GetDefaultEngineData(FVehicleEngineData& OutEngineData ) const;
};
