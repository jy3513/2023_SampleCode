// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Object.h"

#include "AchievementmentList.generated.h"

enum class EAchieveConditionType : uint8;

UENUM(BlueprintType)
enum class EAchieveConditionType : uint8
{
	None,
	Day
	Friend,
	Shop,
	Advertisement,
};

UCLASS()
class CLIENT_R_API UAchievement : public UObject
{
	GENERATED_BODY()
public:
	int32 GetAchievementID() const { return m_AchievementID; }
	int32 GetCount() const { return m_Count; }
	bool IsComplete() const { return m_IsComplete; }

public:
	void UpdateAchieve(const int32& InAchieveID, const int32& InCount, const bool InbComplete);

private:
	int32 m_AchievementID = 0;
	int32 m_Count = 0;
	bool m_IsComplete = false;
};

UCLASS()
class CLIENT_R_API AchievementSystem : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Start() override;
	virtual void Release() override;

public:
	/**
	* Temp.
	* InConditionType : 업적 조건
	* InCount : 추가로 더해줄 값.
	*/
	void CheckAchieve(const EAchieveConditionType& InConditionType, const int32& InCount);

public:
	void SetAchieve(const int32& InCompleteAchieveID, const int32& InGroupID, const int32& InCount);

public:
	UAchievement* GetAchieve(const int32& InGroupID);

private:
	bool LoadTable();
	void SendAchieve(const int32& InAchieveID, const int32& InCount);

private:
	UPROPERTY()	UDataTable* m_AchieveTable;
	UPROPERTY()	UDataTable* m_AchieveGroupTable;
	UPROPERTY() TArray<UAchievement*> m_AchieveList;

};
#pragma once
