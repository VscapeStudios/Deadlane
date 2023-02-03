// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/DEVehicleMovement.h"
#include "Vehicle/DEWheeledVehicle.h"


#include "PhysXPublic.h"

UDEVehicleMovement::UDEVehicleMovement(const FObjectInitializer& ObjectInitializer) 
: Super(ObjectInitializer)
{

}

bool UDEVehicleMovement::IsWheelDrive(const FWheelSetup& WheelSetup)
{
	switch (DifferentialSetup.DifferentialType)
	{
	default:
		return false;
		break;
	case EVehicleDifferential4W::LimitedSlip_4W:
	case EVehicleDifferential4W::Open_4W:
		return true;
		break;
	case EVehicleDifferential4W::LimitedSlip_RearDrive:
	case EVehicleDifferential4W::Open_RearDrive:
		return GetWheelRestingPosition(WheelSetup).X < 0.0f;
		break;
	case EVehicleDifferential4W::LimitedSlip_FrontDrive:
	case EVehicleDifferential4W::Open_FrontDrive:
		return GetWheelRestingPosition(WheelSetup).X > 0.0f;
		break;
	}
}


float UDEVehicleMovement::GetCurrentMaxRPM() const
{
	float CurrentRPM = 0.0f;
#if WITH_PHYSX
	if (PVehicleDrive)
	{
		PxVehicleDrive4W* PVehicleDrive4W = (PxVehicleDrive4W*)PVehicleDrive;
		const physx::PxVehicleEngineData& CurrentEngineData = PVehicleDrive4W->mDriveSimData.getEngineData();
		CurrentRPM = OmegaToRPM(CurrentEngineData.mMaxOmega);
	}
#endif // WITH_PHYSX

	return CurrentRPM;
}

void UDEVehicleMovement::SetMaxRPM(float NewMaxRPM)
{	

	if (GetCurrentMaxRPM() != NewMaxRPM)
	{
		FVehicleEngineData EngineData;
		GetDefaultEngineData(EngineData);

		//Apply MaxRPM
		EngineData.MaxRPM = NewMaxRPM;

		//Preserve Torque
		const float CurrentTorqueFactor = GetTorqueFactor();

		TArray<FRichCurveKey> TorqueKeys = EngineData.TorqueCurve.GetRichCurve()->GetCopyOfKeys();
		for (int32 KeyIdx = 0; KeyIdx < TorqueKeys.Num(); ++KeyIdx)
		{
			const float NewValue = TorqueKeys[KeyIdx].Value * CurrentTorqueFactor;
			EngineData.TorqueCurve.GetRichCurve()->UpdateOrAddKey(TorqueKeys[KeyIdx].Time, NewValue);
		}

		//UE_LOG(LogTemp, Warning, TEXT("SetMaxRPM. EngineData.MaxRPM: %f, ZeroTorque: %f"), EngineData.MaxRPM, EngineData.TorqueCurve.GetRichCurve()->Eval(0.0f));
		UpdateEngineSetup(EngineData);
	}
	
}

float UDEVehicleMovement::GetTorqueFactor() const
{
	float CurrentPeakTorque = 0.0f;
#if WITH_PHYSX
	if (PVehicleDrive)
	{
		PxVehicleDrive4W* PVehicleDrive4W = (PxVehicleDrive4W*)PVehicleDrive;
		const physx::PxVehicleEngineData& CurrentEngineData = PVehicleDrive4W->mDriveSimData.getEngineData();
		CurrentPeakTorque = Cm2ToM2(CurrentEngineData.mPeakTorque);
	}
#endif // WITH_PHYSX
	
	FVehicleEngineData EngineData;
	GetDefaultEngineData(EngineData);

	// Find max torque
	float DefaultPeakTorque = 0.f;
	TArray<FRichCurveKey> TorqueKeys = EngineData.TorqueCurve.GetRichCurveConst()->GetCopyOfKeys();
	for (int32 KeyIdx = 0; KeyIdx < TorqueKeys.Num(); KeyIdx++)
	{
		FRichCurveKey& Key = TorqueKeys[KeyIdx];
		DefaultPeakTorque = FMath::Max(DefaultPeakTorque, Key.Value);
	}

	return DefaultPeakTorque == 0.0f ? 0.0f : CurrentPeakTorque / DefaultPeakTorque;
	
}

void UDEVehicleMovement::SetTorqueFactor(float NewTorqueFactor/*= 1.0f*/)
{
		float CurrentRPM = GetCurrentMaxRPM();

		FVehicleEngineData EngineData;
		GetDefaultEngineData(EngineData);

		//preserve current MaxRPM
		if (CurrentRPM > 0.0f)
		{
			EngineData.MaxRPM = CurrentRPM;
		}


		//ApplyTorque
		TArray<FRichCurveKey> TorqueKeys = EngineData.TorqueCurve.GetRichCurve()->GetCopyOfKeys();
		for (int32 KeyIdx = 0; KeyIdx < TorqueKeys.Num(); ++KeyIdx)
		{
			const float NewValue = TorqueKeys[KeyIdx].Value * NewTorqueFactor;
			EngineData.TorqueCurve.GetRichCurve()->UpdateOrAddKey(TorqueKeys[KeyIdx].Time, NewValue);
		}

		//UE_LOG(LogTemp, Warning, TEXT("UpdateEngineFactors. EngineData.MaxRPM: %f, ZeroTorque: %f"), EngineData.MaxRPM, EngineData.TorqueCurve.GetRichCurve()->Eval(0.0f));
		UpdateEngineSetup(EngineData);
}

float UDEVehicleMovement::GetVehicleMaxSpeed()
{
	float MaxDriveWheelRadius = 0.0f;
	for (UVehicleWheel* Wheel : Wheels)
	{
		if (Wheel && IsWheelDrive(Wheel->GetWheelSetup()))
		{	
			MaxDriveWheelRadius = FMath::Max(MaxDriveWheelRadius, Wheel->ShapeRadius);	
		}
	}

	const float LastGearRatio = TransmissionSetup.ForwardGears.Num() > 0 ? TransmissionSetup.ForwardGears.Last().Ratio : 0.0f;
	
	const float MaxRatio = LastGearRatio * TransmissionSetup.FinalRatio;
	if (MaxRatio == 0.0f)
	{
		return 0.0f;
	}

	return GetCurrentMaxRPM() / MaxRatio * 2 * PI * MaxDriveWheelRadius * 0.0006f;

}

float UDEVehicleMovement::GetDefaultVehicleMaxSpeed()
{
	float MaxDriveWheelRadius = 0.0f;
	for (UVehicleWheel* Wheel : Wheels)
	{
		if (Wheel && IsWheelDrive(Wheel->GetWheelSetup()))
		{
			MaxDriveWheelRadius = FMath::Max(MaxDriveWheelRadius, Wheel->ShapeRadius);
		}
	}

	const float LastGearRatio = TransmissionSetup.ForwardGears.Num() > 0 ? TransmissionSetup.ForwardGears.Last().Ratio : 0.0f;

	const float MaxRatio = LastGearRatio * TransmissionSetup.FinalRatio;
	if (MaxRatio == 0.0f)
	{
		return 0.0f;
	}

	return MaxEngineRPM / MaxRatio * 2 * PI * MaxDriveWheelRadius * 0.0006f;
}

void UDEVehicleMovement::SetVehicleMaxSpeed(float NewMaxSpeed)
{
	float MaxDriveWheelRadius = 0.0f;
	for (UVehicleWheel* Wheel : Wheels)
	{
		if (Wheel && IsWheelDrive(Wheel->GetWheelSetup()))
		{
			MaxDriveWheelRadius = FMath::Max(MaxDriveWheelRadius, Wheel->ShapeRadius);
		}
	}

	const float LastGearRatio = TransmissionSetup.ForwardGears.Num() > 0 ? TransmissionSetup.ForwardGears.Last().Ratio : 0.0f;

	const float MaxRatio = LastGearRatio * TransmissionSetup.FinalRatio;

	const float TargetMaxRPM = NewMaxSpeed / 0.0006f / MaxDriveWheelRadius / PI / 2 * MaxRatio;

	SetMaxRPM(TargetMaxRPM);

}


void UDEVehicleMovement::ResetVehicleMaxSpeed()
{
	SetMaxRPM(MaxEngineRPM);
}

void UDEVehicleMovement::GetDefaultEngineData(FVehicleEngineData& OutEngineData) const
{
	ADEWheeledVehicle* OwningVehicle = Cast<ADEWheeledVehicle>(GetOwner());
	if (OwningVehicle)
	{
		ADEWheeledVehicle* ArchetypeVehicle = Cast<ADEWheeledVehicle>(OwningVehicle->GetArchetype());
		if (ArchetypeVehicle) 
		{
			UDEVehicleMovement* DefaultMovementComponent = Cast<UDEVehicleMovement>(ArchetypeVehicle->GetVehicleMovementComponent());
			if (DefaultMovementComponent)
			{
				OutEngineData = DefaultMovementComponent->EngineSetup;
				return;
			}
		}
	}
	OutEngineData = FVehicleEngineData();
}