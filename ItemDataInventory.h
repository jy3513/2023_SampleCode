// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Object.h"
#include "ItemDataInventory.generated.h"

/**
 *
 */

class UItemData;

UCLASS()
class UItemDataInventory : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Release() override;
	virtual void Clear() override;

public:
	UItemData* AddItem(const int64& InItemUID, const int32& InDataID, const int32& InCount);

	const bool RemoveItem(const int64& InItemUID, const int32& InCount) const;

public:
	UItemData* GetItemByUID(const int64& InItemUID) const;

	const bool GetItemListByDataID(const int32& InDataID, TArray<UItemData*>& OutItemList) const;

	const bool GetAllItems(TArray<UItemData*>& OutItemList) const;

private:
	UPROPERTY(Transient)
		TArray<UItemData*> m_ItemList;

};

#pragma once
