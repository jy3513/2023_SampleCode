// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

#include "Object.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

#include "ReddotSystem.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class ERedDotConditions : uint8
{
	None,
	Achievements,
	Mail, 
	Exchange, 
	Inventory, 
	Friend, 
	Quest, 
	Guild,
	Shop,
	AccountSkill,
};

USTRUCT(BlueprintType)
struct FReddotNotiInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		ERedDotConditions RedDotConditions;

public:
	FReddotNotiInfo()
		:RedDotConditions(ERedDotConditions::None),
	{};

	FReddotNotiInfo(ERedDotConditions InRedDotConditions, int64 InTargetIndex)
		:RedDotConditions(InRedDotConditions),
	{};
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnContentReddotEvent, const ERedDotConditions&);

UCLASS()
class UReddotSystem : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Start() override;
	virtual void Release() override;
	//=========================================================================================================

#pragma region Get_Reddot
public:
	/**
	* 해당 컨텐츠에 레드닷 정보가 있는지 확인.
	* @param ERedDotConditions 레드닷 컨텐츠 타입.
	*/
	bool GetReddot(const ERedDotConditions& InRedDotConditions);

	/**
	* 해당 컨텐츠에 레드닷 정보가 있는지 확인.
	* @param ERedDotConditions 레드닷 컨텐츠 타입.
	* @param TArray<int64> 해당 레드닷 타입에 띄울 정보.
	*/
	bool GetReddotInfo(const ERedDotConditions& InContentEnum, TArray<int64>& OutReddotList);
#pragma endregion

#pragma region Set_Reddot
public:
	/**
	* 새롭게 레드닷을 띄워야 하는 컨텐츠.
	* @param ERedDotConditions 레드닷 컨텐츠 타입.
	* @param int64 레드닷 조건.
	*/
	bool SetReddot(const ERedDotConditions& InRedDotConditions, const int64& InTargetIndex);
	
	/**
	* 레드닷을 더이상 띄우지 않아도 될 때 호출.
	* @param ERedDotConditions 레드닷 컨텐츠.
	* @param int64 레드닷 조건.
	*/
	bool RemoveContentReddot(const ERedDotConditions& InContentEnum, const int64& InTargetIndex);
#pragma endregion

	//=========================================================================================================

#pragma region Save_Reddot
private:
	void SaveAsync();
	void LoadAsync();

	UFUNCTION() void LoadGameFromSlot(const FString& InUserName, const int32 InUserIndex, USaveGame* InSaveGame);
#pragma endregion

public:
	FOnContentReddotEvent m_OnContentReddot;

private:
	const FString m_ReddotDataKey = TEXT("_Reddot");

	UPROPERTY()		UReddotSystemData* m_ReddotData = nullptr;

};