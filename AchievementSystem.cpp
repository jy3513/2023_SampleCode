// Fill out your copyright notice in the Description page of Project Settings.

#include "AchievementSystem.h"

void UAchievement::UpdateAchieve(const int32& InAchieveID, const int32& InCount, const bool InbComplete)
{
	m_AchievementID = InAchieveID;
	m_Count = InCount;
	m_IsComplete = InbComplete;
}

void UAchievementSystem::Initialize()
{
}

void UAchievementSystem::Start()
{
}

void UAchievementSystem::Release()
{
	m_AchieveList.Empty();
}

void UAchievementSystem::CheckAchieve(const EAchieveConditionType& InConditionType, const int32& InCount)
{
	if (InConditionType == EAchieveConditionType::None || InCount <= 0)
	{
		return;
	}

	if (m_AchieveTable == nullptr || m_AchieveTable->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchieveTable is nullptr"));
		return;
	}

	if (m_AchieveGroupTable == nullptr || m_AchieveGroupTable->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchieveGroupTable is nullptr"));
		return;
	}

	//=========================================================================================================
	TArray<FAchieveGroupData*> AchievementGroupList;
	m_AchieveGroupTable->GetAllRows<FAchieveGroupData>(TEXT("FAchieveGroupData"), AchievementGroupList);

	for (const FAchieveGroupData* AchieveGroupData : AchievementGroupList)
	{
		if (AchieveGroupData == nullptr || AchieveGroupData->IsValidLowLevel() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("FAchieveGroupData is nullptr"));
			return;
		}

		FAchieveData* AchieveData = m_AchieveTable->FindRow<FAchieveData>(AchieveGroupData->start_achievment_idx, TEXT("FAchieveData"));
		if (AchieveData == nullptr || AchieveData->IsValidLowLevel() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AchieveData is nullptr [ index : %d ]"), AchieveGroupData->start_achievment_idx);
			return;
		}

		// 타입 확인.
		if (InConditionType != AchieveData->condition_type)
		{
			continue;
		}

		UAchievement* AchieveInfo = GetAchieve(AchieveGroupID);
		if (AchieveInfo == nullptr || AchieveInfo->IsValidLowLevel() == false)
		{
			SendAchieve(AchieveGroupData->start_achievment_idx, InCount);
		}
		else
		{
			SendAchieve(AchieveInfo->GetAchievementID(), InCount);
		}

		break;
	}
}

void UAchievementSystem::SetAchieve(const int32& InCompleteAchieveID, const int32& InGroupID, const int32& InCount)
{
	if (m_AchieveTable == nullptr || m_AchieveTable->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchieveTable is nullptr"));
		return;
	}

	UAchievement* AchieveInfo = GetAchieve(InGroupID);
	if (AchieveInfo == nullptr || AchieveInfo->IsValidLowLevel() == false)
	{
		AchieveInfo = NewObject<UAchievement>(this);
		m_AchieveList.Emplace(AchieveInfo);
	}
	
	if (AchieveInfo == nullptr || AchieveInfo->IsValidLowLevel() == false)
	{
		return;
	}

	if (InCompleteAchieveID > 0)
	{
		FAchieveData* AchieveData = m_AchieveTable->FindRow<FAchieveData>(InCompleteAchieveID, TEXT("FAchieveData"));
		if (AchieveData == nullptr || AchieveData->IsValidLowLevel() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AchieveData is nullptr [ index : %d ]"), InCompleteAchieveID);
			return;
		}

		// 다음 업적이 없음.
		if (AchieveData->next_idx == 0)
		{
			AchieveInfo->UpdateAchieve(AchieveData->achievment_idx, InGroupID, InCount, true);
		}
		else
		{
			AchieveInfo->UpdateAchieve(AchieveData->next_idx, InGroupID, InCount, false);
		}
	}
	else
	{
		AchieveInfo->UpdateAchieve(InGroupID, InGroupID, InCount, false);
	}
}

UAchievement* UAchievementSystem::GetAchieve(const int32& InGroupID)
{
	if (InGroupID == 0)
	{
		return nullptr;
	}

	UAchievement** ResultInfo = m_AchieveList.FindByPredicate([InGroupID](const UAchievement* AchieveInfo)
		{
			if (AchieveInfo == nullptr || AchieveInfo->IsValidLowLevel() == false)
			{
				return false;
			}
			return AchieveInfo->GetAchievementGroup() == InGroupID;
		});
	
	if (ResultInfo != nullptr)
	{
		return *ResultInfo;
	}
	
	return nullptr;
}

bool UAchievementSystem::LoadTable()
{
	m_AchieveTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprint/DataTable/Achievement.Achievement"));
	if (m_AchieveTable == nullptr || m_AchieveTable->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchieveTable is nullptr"));
		return false;
	}

	m_AchieveGroupTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprint/DataTable/AchievementGroup.AchievementGroup"));
	if (m_AchieveGroupTable == nullptr || m_AchieveGroupTable->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchieveGroupTable is nullptr"));
		return false;
	}

	return true;
}

void UAchievementSystem::SendAchieve(const int32& InAchieveID, const int32& InCount)
{
	//	TODO : network send.
}
