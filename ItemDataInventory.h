// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Object.h"
#include "ItemDataInventory.generated.h"

/**
 *
 */

class UItemData;

enum class EItemType : uint8;

DECLARE_MULTICAST_DELEGATE(FInventoryUpdateEvent);

UCLASS()
class UItemDataInventory : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Release() override;
	virtual void Clear() override;

#pragma region Item
public:
	UItemData* AddItem(const int64& InItemUID, const int32& InDataID, const int32& InCount);

	const bool RemoveItem(const int64& InItemUID, const int32& InCount) const;


public:
	UItemData* GetItemByUID(const int64& InItemUID) const;

	const bool GetItemListByDataID(const int32& InDataID, TArray<UItemData*>& OutItemList) const;

	const bool GetItemListByItemType(const EItemType& InItemType, TArray<UItemData*>& OutItemList) const;

	const bool GetAllItems(TArray<UItemData*>& OutItemList) const;


#pragma endregion Item

//=========================================================================================================

#pragma region InventoryCount
public:
	const bool SetInventoryCount(const int32& InInventoryCount) { InventoryCount = InInventoryCount; } const;


public:
	const int32 GetInventoryCount() { return InventoryCount; }const;


#pragma endregion InventoryCount

public:
	FInventoryUpdateEvent InventoryUpdate;


private:
	UPROPERTY(Transient)
		TArray<UItemData*> m_ItemList;

	UPROPERTY(Transient)
		int32 InventoryCount = 0;

	UPROPERTY(Transient)
		FInventoryUpdateEvent FInventoryUpdateEvent;

};

#pragma once
