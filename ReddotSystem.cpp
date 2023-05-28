// Fill out your copyright notice in the Description page of Project Settings.

#include "ReddotSystem.h"

void UReddotSystem::Initialize()
{

}

void UReddotSystem::Start()
{
    LoadAsync();
}

void UReddotSystem::Release()
{
    m_ReddotData = nullptr;
}

// public.
//===============================================================================================================================================================================
// public - Content Reddot.

#pragma region Get_Reddot
bool UReddotSystem::GetReddot(const ERedDotConditions& InRedDotConditions)
{
    if (ERedDotConditions::None == InRedDotConditions)
    {
        UE_LOG(LogTemp, Warning, TEXT("InRedDotConditions is None"));
        return false;
    }

    if (m_ReddotData == nullptr || m_ReddotData->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("m_ReddotData is nullptr"));
        return false;
    }

    // get data.
    TArray<int64> ReddotList;
    return GetReddotInfo(InContentEnum, ReddotList);
}

bool UReddotSystem::GetReddotInfo(const ERedDotConditions& InContentEnum, TArray<int64>& OutReddotList)
{
    if (InContentEnum == ERedDotConditions::None)
    {
        UE_LOG(LogTemp, Warning, TEXT("InContentEnum is None"));
        return false;
    }

    if (m_ReddotData == nullptr || m_ReddotData->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("UReddotSystemData is nullptr"));
        return false;
    }
    
    OutReddotList.Empty();
    
    for (const FReddotNotiInfo& Contents : m_ReddotData->ContentReddotList)
    {
        if (InContentEnum != Contents.RedDotConditions)
        {
            continue;
        }

        OutReddotList.Emplace(Contents.TargetIndex);
    }

    return (bool)(OutReddotList.Num() > 0);
}

#pragma endregion

#pragma region Set_Reddot
bool UReddotSystem::SetReddot(const ERedDotConditions& InRedDotConditions, const int64& InTargetIndex)
{
    if (InRedDotConditions == ERedDotConditions::None)
    {
        UE_LOG(LogTemp, Warning, TEXT("InRedDotConditions is None"));
        return false;
    }
    
    if (m_ReddotData == nullptr || m_ReddotData->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("UReddotSystemData is nullptr"));
        return false;
    }

    // check.
    for (const FReddotNotiInfo& Contents : m_ReddotData->ContentReddotList)
    {
        if (Contents.RedDotConditions != InRedDotConditions ||
            Contents.TargetIndex != InTargetIndex)
        {
            continue;
        }

        // 동일한게 있음.
        return false;
    }

    // emplace.
    m_ReddotData->ContentReddotList.Emplace(FReddotNotiInfo(InRedDotConditions, InTargetIndex));
    
    // broadcast.
    m_OnContentReddot.Broadcast(InRedDotConditions);
    
    // save.
    SaveAsync();
    
    return true;
}

bool UReddotSystem::RemoveContentReddot(const ERedDotConditions& InContentEnum, const int64& InTargetIndex)
{
    // RewardReceiving 타입의 레드닷을 제거해줌.
    if (InContentEnum == ERedDotConditions::None)
    {
        UE_LOG(LogTemp, Warning, TEXT("InContentEnum is None"));
        return false;
    }

    if (m_ReddotData == nullptr || m_ReddotData->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UReddotSystemData] is null"));
        return false;
    }

    m_ReddotData->ContentReddotList.RemoveAll([this, InContentEnum, InTargetIndex](const FReddotNotiInfo& InInfo)
        {
            return InContentEnum == InInfo.RedDotConditions
                && InTargetIndex == InInfo.TargetIndex;
        });

    // save.
    SaveAsync();

    return true;
}

#pragma endregion

// public - Content Reddot.
//===============================================================================================================================================================================
// private.

#pragma region Save_Reddot
void UReddotSystem::SaveAsync()
{
    UGameplayStatics::AsyncSaveGameToSlot(m_ReddotData, m_ReddotDataKey, 0);
}

void UReddotSystem::LoadAsync()
{
    FAsyncLoadGameFromSlotDelegate AsyncLoadGameFromSlotDelegate;
    AsyncLoadGameFromSlotDelegate.BindUFunction(this, TEXT("LoadGameFromSlot"));

    UGameplayStatics::AsyncLoadGameFromSlot(m_ReddotDataKey, 0, AsyncLoadGameFromSlotDelegate);
}

void UReddotSystem::LoadGameFromSlot(const FString& InUserName, const int32 InUserIndex, USaveGame* InSaveGame)
{
    m_ReddotData = Cast<UReddotSystemData>(InSaveGame);

    if (m_ReddotData == nullptr || m_ReddotData->IsValidLowLevel() == false)
    {
        // 없으면 새로 만듦.
        m_ReddotData = Cast<UReddotSystemData>(UGameplayStatics::CreateSaveGameObject(UReddotSystemData::StaticClass()));
    }
}

#pragma endregion
