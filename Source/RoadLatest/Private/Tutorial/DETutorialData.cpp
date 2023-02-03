// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial/DETutorialData.h"

bool UDETutorialData::IsValidTutorialID(ETutorialType InTutorialType, int32 InTutorialID) const
{	
	switch (InTutorialType)
	{
	case ETutorialType::Sequental:
		return SequentialTutorials.IsValidIndex(InTutorialID);
		break;
	case ETutorialType::OnDemand:
		return OnDemandTutorials.IsValidIndex(InTutorialID);
		break;
	default:
		return false;
		break;
	}
}
