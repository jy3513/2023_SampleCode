// Fill out your copyright notice in the Description page of Project Settings.

#include "AchievementSystem.h"

//	������ �⺻ �ܰ�� 1�� ����.
#define DEFAULT_ACHIEVEMENT_STEP 1

void UAchievement::UpdateAchieve(const int32& InStep, const int32& InCount, const bool InbAllComplete)
{
	Step = InStep;
	Count = InCount;
	IsAllComplete = InbAllComplete;
}

void UAchievementSystem::Initialize()
{
}

void UAchievementSystem::Start()
{
}

void UAchievementSystem::Release()
{
	AchieveList.Empty();
}

bool UAchievementSystem::SetAchievementData(const EAchieveConditionType& InConditionType, const int32& InStep, const int32& InCount)
{
	if (InConditionType == EAchieveConditionType::None)
	{
		return false;
	}

	if (AchieveTable == nullptr || AchieveTable->IsValidLowLevel() == false)
	{
		//	���̺��� ������ �ٽ� �ҷ���.
		if (LoadTable() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AchieveTable is nullptr"));
			return false;
		}
	}

	//	���� ���̺��� '����Ÿ��_�����ܰ�' �� ã�� �� ����.
	const FName AchievementTableID = FName(*FString::Printf(TEXT("%d_%d"), static_cast<int32>(InConditionType), InStep));
	FAchievementData* AchievementData = AchieveTable->FindRow<FAchievementData>(AchievementTableID, TEXT("FAchievementData"));
	if (AchievementData == nullptr || AchievementData->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchievementData is nullptr [ index : %s ]"), *AchievementTableID.ToString());
		return false;
	}

	//	����� �����Ͱ� ������ ���� ����.
	if (AchieveList.Contains(InConditionType) == false || AchieveList[InConditionType] == nullptr || AchieveList[InConditionType]->IsValidLowLevel() == false)
	{
		AchieveList.Emplace(InConditionType, NewObject<UAchievement>(this));
	}

	//	���� ���̺��� next_step�� �̿��� ���� �ܰ谡 �ִ��� Ȯ�� �� ����.
	AchieveList[InConditionType]->UpdateAchieve(InStep, InCount, (bool)(AchievementData->next_step <= 0));

	return true;
}

const UAchievement* UAchievementSystem::GetAchievementData(const EAchieveConditionType& InConditionType)
{
	if (InConditionType == EAchieveConditionType::None)
	{
		return nullptr;
	}

	if (AchieveList.Contains(InConditionType) == false)
	{
		return nullptr;
	}

	return AchieveList[InConditionType];
}

bool UAchievementSystem::UpdateAchievement(const EAchieveConditionType& InConditionType, const int32& InCount)
{
	if (InConditionType == EAchieveConditionType::None || InCount <= 0)
	{
		return false;
	}

	//=========================================================================================================
	//	����Ǿ� �ִ� ������ �ִ��� Ȯ��.
	if (AchieveList.Contains(InConditionType))
	{
		if (AchieveList[InConditionType] != nullptr && AchieveList[InConditionType]->IsValidLowLevel())
		{
			if (AchieveList[InConditionType]->IsAllComplete())
			{
				//	�̹� �Ϸ� ó�� �Ǿ ���̻� ���� ���� �ø� �� ����.
				return false;
			}
		}
	}

	//	�Ϸ� ó���� ���� �ʾ����� ������ Req ����.
	return SendAchievement_Update(InConditionType, InCount);
}

bool UAchievementSystem::PossibleCompleteAchievement(const EAchieveConditionType& InConditionType)
{
	if (InConditionType == EAchieveConditionType::None)
	{
		return false;
	}

	//	����Ǿ� �ִ� ������ ������ �Ϸ� ó�� �� �� ����.
	if (AchieveList.Contains(InConditionType) == false || AchieveList[InConditionType] == nullptr || AchieveList[InConditionType]->IsValidLowLevel() == false)
	{
		return false;
	}

	//	�̹� ��� ������ �Ϸ� ó�� �Ǿ� ����.
	if (AchieveList[InConditionType]->IsAllComplete())
	{
		return false;
	}

	//	���̺��� �� Ȯ�� �ʿ�.
	if (AchieveTable == nullptr || AchieveTable->IsValidLowLevel() == false)
	{
		//	���̺��� ������ �ٽ� �ҷ���.
		if (LoadTable() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AchieveTable is nullptr"));
			return false;
		}
	}

	//	���� ���̺��� '����Ÿ��_�����ܰ�' �� ã�� �� ����.
	const FName AchievementTableID = FName(*FString::Printf(TEXT("%d_%d"), static_cast<int32>(InConditionType), AchieveList[InConditionType]->GetStep()));
	FAchievementData* AchievementData = AchieveTable->FindRow<FAchievementData>(AchievementTableID, TEXT("FAchievementData"));
	if (AchievementData == nullptr || AchievementData->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchievementData is nullptr [ index : %s ]"), *AchievementTableID.ToString());
		return false;
	}

	//	���̺��� ������ ū �� Ȯ��.
	return (bool)(AchievementData->condition_count <= AchieveList[InConditionType]->GetCount());
}

bool UAchievementSystem::SendAchievement_Update(const EAchieveConditionType& InConditionType, const int32& InCount)
{
	if (InConditionType == EAchieveConditionType::None || InCount <= 0)
	{
		return false;
	}

	//	TODO : network send.

	return false;
}

bool UAchievementSystem::SendAchievement_Complete(const EAchieveConditionType& InConditionType)
{
	if (PossibleCompleteAchievement(InConditionType) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("PossibleCompleteAchievement is false."));
		return false;
	}

	//	TODO : network send.

	return false;
}

//	public.
//===============================================================================================================================================================================
//	private.

bool UAchievementSystem::LoadTable()
{
	AchieveTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprint/DataTable/Achievement.Achievement"));
	if (AchieveTable == nullptr || AchieveTable->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchieveTable is nullptr"));
		return false;
	}

	return true;
}

#undef DEFAULT_ACHIEVEMENT_STEP