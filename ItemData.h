// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Object.h"
#include "ItemData.generated.h"

/**
 *
 */

UCLASS()
class UItemData : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Start() override;
	virtual void Release() override;

public:
	void SetUID(const int64& InUID) { m_UID = InUID; }
	void SetDataID(const int32& InDataID) { m_DataID = InDataID; }

	void SetItemCount(const int32& InCount) { ItemCount = InCount; }

public:
	const int64 GetUID() const { return m_UID; }
	const int32 GetDataID() const { return m_DataID; }

	const int32 GetItemCount() const { return ItemCount; }
	

private:
	int64 m_UID = 0;
	int32 m_DataID = 0;

	int32 m_Count = 0;

};

#pragma once
