// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"

#include "AchievementmentList.generated.h"

/**
 * ���� �ý���.
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
	* �������� ���� ���� �Ǿ��� �� ���ο� ���� ����.
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
	* �������� Recv�� ������ �� ����� ���� ����.
	* �� �ܿ��� ȣ������ ����.
	*/
	bool SetAchievementData(const EAchieveConditionType& InConditionType, const int32& InStep, const int32& InCount);

	/**
	* ����� �����͸� Ȯ���ؾ��� �� ȣ��.
	*/
	const UAchievement* GetAchievementData(const EAchieveConditionType& InConditionType);


public:

	/**
	* Ŭ�󿡼� ���� Ÿ�Կ� ���� ���� ���� �Ǿ�� �� �� ȣ���ϴ� �Լ�.
	* InConditionType : ���� ����
	* InCount : �߰��� ������ ��.
	*/
	bool UpdateAchievement(const EAchieveConditionType& InConditionType, const int32& InCount);
	
	/**
	* ���� ������ ���� �� �ִ��� Ȯ��.
	* InConditionType : ���� ����
	*/
	bool PossibleCompleteAchievement(const EAchieveConditionType& InConditionType);




public:
	/**
	* ������ ����� ���� Ÿ�԰� ���� Req �ϴ� �Լ�.
	* Req�� ����� ������ ������ �� false.
	*/
	bool SendAchievement_Update(const EAchieveConditionType& InConditionType, const int32& InCount);

	/**
	* �Ϸ�� ������ ������ �ޱ� ���� ������ Req�� ����.
	* Req�� ����� ������ ������ �� false.
	*/
	bool SendAchievement_Complete(const EAchieveConditionType& InConditionType);


private:
	/**
	* AchieveTable DataTable�� �ҷ���.
	*/
	bool LoadTable();


private:
	UPROPERTY()	UDataTable* AchieveTable;
	UPROPERTY() TMap<EAchieveConditionType, UAchievement*> AchieveList;

};


