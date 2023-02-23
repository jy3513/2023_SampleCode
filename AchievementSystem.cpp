// Fill out your copyright notice in the Description page of Project Settings.

#include "AchievementSystem.h"

//	업적의 기본 단계는 1로 정함.
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
		//	테이블이 없으면 다시 불러옴.
		if (LoadTable() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AchieveTable is nullptr"));
			return false;
		}
	}

	//	업적 테이블은 '업적타입_업적단계' 로 찾을 수 있음.
	const FName AchievementTableID = FName(*FString::Printf(TEXT("%d_%d"), static_cast<int32>(InConditionType), InStep));
	FAchievementData* AchievementData = AchieveTable->FindRow<FAchievementData>(AchievementTableID, TEXT("FAchievementData"));
	if (AchievementData == nullptr || AchievementData->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchievementData is nullptr [ index : %s ]"), *AchievementTableID.ToString());
		return false;
	}

	//	저장된 데이터가 없으면 새로 만듦.
	if (AchieveList.Contains(InConditionType) == false || AchieveList[InConditionType] == nullptr || AchieveList[InConditionType]->IsValidLowLevel() == false)
	{
		AchieveList.Emplace(InConditionType, NewObject<UAchievement>(this));
	}

	//	업적 테이블의 next_step를 이용해 다음 단계가 있는지 확인 및 저장.
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
	//	저장되어 있는 업적이 있는지 확인.
	if (AchieveList.Contains(InConditionType))
	{
		if (AchieveList[InConditionType] != nullptr && AchieveList[InConditionType]->IsValidLowLevel())
		{
			if (AchieveList[InConditionType]->IsAllComplete())
			{
				//	이미 완료 처리 되어서 더이상 업적 값을 올릴 수 없음.
				return false;
			}
		}
	}

	//	완료 처리가 되지 않았으면 서버로 Req 보냄.
	return SendAchievement_Update(InConditionType, InCount);
}

bool UAchievementSystem::PossibleCompleteAchievement(const EAchieveConditionType& InConditionType)
{
	if (InConditionType == EAchieveConditionType::None)
	{
		return false;
	}

	//	저장되어 있는 업적이 없으면 완료 처리 할 수 없음.
	if (AchieveList.Contains(InConditionType) == false || AchieveList[InConditionType] == nullptr || AchieveList[InConditionType]->IsValidLowLevel() == false)
	{
		return false;
	}

	//	이미 모든 업적이 완료 처리 되어 있음.
	if (AchieveList[InConditionType]->IsAllComplete())
	{
		return false;
	}

	//	테이블의 값 확인 필요.
	if (AchieveTable == nullptr || AchieveTable->IsValidLowLevel() == false)
	{
		//	테이블이 없으면 다시 불러옴.
		if (LoadTable() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AchieveTable is nullptr"));
			return false;
		}
	}

	//	업적 테이블은 '업적타입_업적단계' 로 찾을 수 있음.
	const FName AchievementTableID = FName(*FString::Printf(TEXT("%d_%d"), static_cast<int32>(InConditionType), AchieveList[InConditionType]->GetStep()));
	FAchievementData* AchievementData = AchieveTable->FindRow<FAchievementData>(AchievementTableID, TEXT("FAchievementData"));
	if (AchievementData == nullptr || AchievementData->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AchievementData is nullptr [ index : %s ]"), *AchievementTableID.ToString());
		return false;
	}

	//	테이블의 값보다 큰 지 확인.
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