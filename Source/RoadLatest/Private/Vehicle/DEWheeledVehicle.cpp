// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/DEWheeledVehicle.h"
#include "Vehicle/DEVehicleMovement.h"

ADEWheeledVehicle::ADEWheeledVehicle(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDEVehicleMovement>(VehicleMovementComponentName))
{

}