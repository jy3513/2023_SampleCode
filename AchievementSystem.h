// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"

#include "AchievementmentList.generated.h"

/**
 * 업적 시스템.
 */

UENUM(BlueprintType)
enum class EAchieveConditionType : uint8
{
	None,
	GetPoint,
	GetItem,
	Level,
	Day,
	Friend,
	Advertisement,
};

UCLASS()
class UAchievement : public UObject
{
	GENERATED_BODY()

public:
	const int32 GetStep() const { return Step; }
	const int32 GetCount() const { return Count; }
	const bool IsAllComplete() const { return IsAllComplete; }

public:
	/**
	* 서버에서 값이 변경 되었을 때 새로운 값을 저장.
	*/
	void UpdateAchieve(const int32& InStep, const int32& InCount, const bool InbAllComplete);

private:
	int32 Step = 0;
	int32 Count = 0;
	bool IsAllComplete = false;

};

UCLASS()
class AchievementSystem : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Start() override;
	virtual void Release() override;

public:
	/**
	* 서버에서 Recv가 들어왔을 때 변경된 값을 저장.
	* 그 외에는 호출하지 않음.
	*/
	bool SetAchievementData(const EAchieveConditionType& InConditionType, const int32& InStep, const int32& InCount);

	/**
	* 저장된 데이터를 확인해야할 때 호출.
	*/
	const UAchievement* GetAchievementData(const EAchieveConditionType& InConditionType);


public:

	/**
	* 클라에서 업적 타입에 따라 값이 변동 되어야 할 때 호출하는 함수.
	* InConditionType : 업적 조건
	* InCount : 추가로 더해줄 값.
	*/
	bool UpdateAchievement(const EAchieveConditionType& InConditionType, const int32& InCount);
	
	/**
	* 업적 보상을 받을 수 있는지 확인.
	* InConditionType : 업적 조건
	*/
	bool PossibleCompleteAchievement(const EAchieveConditionType& InConditionType);




public:
	/**
	* 서버로 변경된 값의 타입과 값을 Req 하는 함수.
	* Req를 제대로 보내지 못했을 시 false.
	*/
	bool SendAchievement_Update(const EAchieveConditionType& InConditionType, const int32& InCount);

	/**
	* 완료된 업적의 보상을 받기 위해 서버로 Req를 보냄.
	* Req를 제대로 보내지 못했을 시 false.
	*/
	bool SendAchievement_Complete(const EAchieveConditionType& InConditionType);


private:
	/**
	* AchieveTable DataTable을 불러옴.
	*/
	bool LoadTable();


private:
	UPROPERTY()	UDataTable* AchieveTable;
	UPROPERTY() TMap<EAchieveConditionType, UAchievement*> AchieveList;

};


