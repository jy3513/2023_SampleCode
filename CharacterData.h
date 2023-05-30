// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Object.h"
#include "CharacterData.generated.h"

/**
 *
 */

enum class EAttributeType : uint8;
enum class ESkillType : uint8;

UCLASS()
class CLIENT_R_API UCharacterData : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Start() override;
	virtual void Release() override;

public:
	void SetUID(const int64& InUID) { m_UID = InUID; }
	void SetDataID(const int32& InDataID) { m_DataID = InDataID; }

	void SetLevel(const int32& InLevel) { m_Level = InLevel; }
	void SetExp(const int64& InExp) { m_Exp = InExp; }

	void SetStatRemainingPoint(const int32& InStatRemainingPoint) { m_StatRemainingPoint = InStatRemainingPoint; }
	void SetStatPoint(const EAttributeType& InAttributeType, const int32& InPoint) { m_StatPointList.Emplace(InAttributeType, InPoint); }

	void SetSkillLevel(const ESkillType& InSkillType, const int32& InSkillLevel) { m_SkillLevelList.Emplace(InSkillType, InSkillLevel); }

	void SetEquipItem(const int64& InEquipItem);
	void SetUnEquipItem(const int64& InEquipItem);


public:
	const int64 GetUID() const { return m_UID; }
	const int32 GetDataID() const { return m_DataID; }

	const int32 GetHeroLevel() const { return m_Level; }
	const int64 GetHeroExp() const { return m_Exp; }

	const int32 GetStatRemainingPoint() const { return m_StatRemainingPoint; }
	const bool GetStatPointList(TMap<int32, int32>& OutPointList) const;

	const int32 GetSkillLevel(const ESkillType InSkillType) const;
	const bool GetSkillLevelList(TMap<ESkillType, int32>& OutSkillList) const;

	const bool GetEquipItemList(TArray<int64>& OutItemUIDList) const;


private:
	int64 m_UID = 0;
	int32 m_DataID = 0;

	int32 m_Level = 0;
	int64 m_Exp = 0;

	int32 m_StatRemainingPoint = 0;
	TMap<EAttributeType, int32> m_StatPointList;

	TMap<ESkillType, int32> m_SkillLevelList;

	TArray<int64> m_EquipItemUIDList;
};

#pragma once