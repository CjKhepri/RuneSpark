//Copyright 2020 - 2025, Vault Break Studios Pty. Ltd.
//Strider - Part of the 'Vault Break Library' (VBL)
//Written by: Ken Claassen


#include "Components/FootLockManager.h"


FFootLockData::FFootLockData()
{
}

FFootLockData::FFootLockData(const EFootLockState InLockState, const float InRemainingLockTime)
	: LockState(InLockState), RemainingLockTime(InRemainingLockTime)
{
	
}

UFootLockManager::UFootLockManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	FootLockMap.Empty(static_cast<int32>(EFootLockId::Foot8) + 1);
}

void UFootLockManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	for(TPair<EFootLockId, FFootLockData>& FootLockPair : FootLockMap)
	{
		FFootLockData& FootLockData = FootLockPair.Value;

		if(FootLockData.LockState == EFootLockState::TimeLocked)
		{
			FootLockData.RemainingLockTime -= DeltaTime;

			if(FootLockData.RemainingLockTime < 0.0f)
			{
				FootLockData.LockState = EFootLockState::Unlocked;
				FootLockData.RemainingLockTime = 0.0f;
			}
		}
	}
}

void UFootLockManager::LockFoot(const EFootLockId FootId, const float Duration)
{
	FFootLockData& FootLockData = FootLockMap.FindOrAdd(FootId);

	if(Duration > 0.0f)
	{
		FootLockData.LockState = EFootLockState::TimeLocked;
	}
	else
	{
		FootLockData.LockState = EFootLockState::Locked;
	}

	FootLockData.RemainingLockTime = Duration;
}

void UFootLockManager::UnlockFoot(const EFootLockId FootId)
{
	if(FFootLockData* FootLockData = FootLockMap.Find(FootId))
	{
		FootLockData->LockState = EFootLockState::Unlocked;
		FootLockData->RemainingLockTime = 0.0f;
	}
}

bool UFootLockManager::IsFootLocked(const EFootLockId FootId) const
{
	if(const FFootLockData* FootLockData = FootLockMap.Find(FootId))
	{
		return FootLockData->LockState > EFootLockState::Unlocked;
	}

	return false;
}

void UFootLockManager::ResetLockingState()
{
	for(TPair<EFootLockId, FFootLockData>& FootLockPair : FootLockMap)
	{
		FFootLockData& FootLockData = FootLockPair.Value;

		FootLockData.LockState = EFootLockState::Unlocked;
		FootLockData.RemainingLockTime = 0.0f;
	}
}