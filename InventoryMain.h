// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"

#include "InventoryMain.generated.h"

/**
 *
 */

class UWidgetSwitcher;
class UTileView;
class UTextBlock;
class UButton;

class UReddotSystem;

class UItemData;

class UTab_ListItem;

DECLARE_DELEGATE_OneParam(FOnItemClickEvent, int64);
DECLARE_DELEGATE(FOpenDumpFilterEvent);

UENUM()
enum class ETabType : uint8
{
	All = 0,
	Weapon = 1,
	Armor = 2,
	Accessories = 3,
	Consumables = 4,
};

UCLASS()
class UListItem_Item_Data : public UObject
{
	GENERATED_BODY()

public:
	void SetItemData(const UItemData* InItemData) { ItemData = InItemData; }
	void SetSelect(const bool InbSelect) { IsSelect = InbSelect; }
	void SetReddot(const bool InbReddot) { IsReddot = InbReddot; }

public:
	const UItemData* GetItemData() const { return ItemData; }
	const bool IsSelect() const { return IsSelect; }
	const bool IsReddot() const { return IsReddot; }

private:
	UItemData* ItemData = nullptr;
	bool IsSelect = false;
	bool IsReddot = false;
};

UCLASS()
class UInventoryMain : public UObject
{
	GENERATED_BODY()

public:
	enum EInventoryViewType : uint8
	{
		View_Equip,
		View_Dump,
	};

public:
#pragma region Base
	/**
	* ó�� ���� UMG ���� �� ȣ��Ǵ� �Լ�.
	* ��ư�� ���� ������ ���� �� ȣ��Ǵ� �Լ��� �����ϰ� UI�� ������ ������ �����.
	*/
	void Init();

	/**
	* �̺�Ʈ�� ������ �Լ����� ������ ����.
	*/
	void Shutdown();


#pragma endregion

#pragma region Delegate
	// set delegate.
	void SetOnItemClickEvent(FOnItemClickEvent InEvent) { m_OnItemClick = InEvent; }
	void SetOnDumpFilterEvent(FOpenDumpFilterEvent InEvent) { m_OnDumpFilter = InEvent; }


#pragma endregion

private:
#pragma region Start
	/**
	* view type�� ���� ������ ui�� �ٲ���.
	* InViewType : ������ Ÿ��.
	*/
	void BuildDataByUser(const UInventoryMain::EInventoryViewType& InViewType);


#pragma endregion

#pragma region Tab
	/**
	* m_ViewType�� ���缭 ��� ���� ������ �ٲ�.
	*/
	void SetTab();

	/**
	* �� �ٲ�. > ��ȭ, ���� �� �ٸ� â���� ȣ��.
	* InTabType : ������ Ÿ��.
	*/
	void ChangeTab(ETabType InTabType);

	/**
	* ���� Ȱ��ȭ ��Ȱ��ȭ ui ����.
	* InOffTabType : ������ ���� �־ ��Ȱ��ȭ ���·� ���� ���� Ÿ��.
	* InOnTabType : ���Ӱ� Ȱ��ȭ ���·� ���� ���� Ÿ��.
	*/
	void ChangeTabUI(const ETabType& InOffTabType, const ETabType& InOnTabType);


#pragma endregion

#pragma region Count Text
	/**
	* ���� �����ϰ� �ִ� �������� ���� ǥ��.
	*/
	void SetCountText();

	/**
	* ���� ������ ���� ������ �������� ���� ǥ��.
	*/
	void SetDumpCountText();


#pragma endregion

#pragma region Filter
	/**
	* m_ActiveTabType�� ���缭 ��� �� �ִ��� Ȯ��.
	* InItem : Ȯ���� Ÿ�� Data.
	*/
	bool FilterItem(UItemData* InItem);


#pragma endregion

#pragma region Item
	/**
	* listview ������ ����� �����͵��� Pool�� ������ �� ������.
	*/
	void ClearListItems();

	/**
	* ���� �����Ͱ� ���� �� �� ȣ��Ǵ� �Լ�.
	* ��� ������ UI�� ������ ���� ǥ�ð� ���ŵ�.
	*/
	UFUNCTION() void UpdateInventory();

	/**
	* View_Dump�� ���� �� ȣ��.
	* ��� ������ UI�� ������ ���� ǥ�ð� ���ŵ�.
	*/
	void UpdateDump();

	/**
	* InItemUID�� �̿��� �ش� ItemUID�� ���� UI�� ���� ������.
	* InItemUID : Ÿ�� �������� UID.
	*/
	void UpdateItem(const int64& InItemUID);

	/**
	* �����ϰ� �ִ� ��� �������� UI�� ���ŵ�.
	*/
	void UpdateAllItem();

	/**
	* ������ UI�� ���� �߰���.
	* InReddotSystem : �������� ���Ӱ� �߰��Ǿ��� ��� ������� ������ ��.
	* InItemData : ���� �߰��� ������ Data.
	*/
	void AddItem(UReddotSystem* InReddotSystem, UItemData* InItemData);

	/**
	* ������ ������ ����ǰų� ���� ���� ���� �� ����ִ� ĭ�� ������ ���� ���� ������.
	*/
	void SyncEmptyItem();

	/**
	* �������� �������� �� ����Ʈ ����.
	* InItemUID : ����Ʈ�� ������ �������� UID.
	* InbSelect : Ȱ��ȭ, ��Ȱ��ȭ ����.
	*/
	void SelectItem(const int64& InItemUID, const bool InbSelect);


#pragma endregion

#pragma region ListView
	/**
	* ������ UI�� �������� �� ȣ��Ǵ� �Լ�.
	* m_ViewType�� ���缭 �Լ��� ȣ����.
	* InItemObject : ���õ� �������� Data.
	*/
	void Callback_ItemSelect(UObject* InItemObject);

	/**
	* InItemUID: ���õ� �������� UID.
	*/
	void Callback_ItemSelect_ViewEquip(const int64& InItemUID);
	void Callback_ItemSelect_ViewDump(const int64& InItemUID);


#pragma endregion

#pragma region AutoEquip
	/**
	* �ڵ� ������ ������ �� view type�� ���缭 �ٸ� �Լ��� ȣ����.
	*/
	void OnClickAutoEquip();


#pragma endregion

#pragma region Button_Event
private:
	/**
	* ��ư�� ������ �� ȣ��Ǵ� �Լ�.
	*/
	UFUNCTION() void OnClicked_Bt_Auto();
	UFUNCTION() void OnClicked_Bt_Dump();
	UFUNCTION() void OnClicked_Bt_Close_Dump();
	UFUNCTION() void OnClicked_Bt_Filter_Dump();


#pragma endregion

protected:
	//	m_ViewType�� ���� �ٲ���� �ϴ� ����.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWidgetSwitcher* WidgetSwitcher_Popup = nullptr;
	//	m_ViewType�� ���� ������ Data�� �� UTileView.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTileView* TileView_Item = nullptr;
	//	m_ViewType�� ���� ������ Data�� �� UTileView.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTileView* TileView_Dump = nullptr;
	//	���� �����ϰ� �ִ� �������� ������ ǥ��.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTextBlock* Text_Value = nullptr;
	//	��� �ڵ����� ��ư.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Auto = nullptr;


#pragma region View_Equip
	//	������ ��ư. (EInventoryViewType�� Equip���� Dump�� ����.)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Dump = nullptr;


#pragma endregion

#pragma region View_Dump
	//	������ �ݱ� ��ư. (EInventoryViewType�� Dump���� Equip���� ����.)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Close_Dump = nullptr;
	//	���� �������� ������ �������� ���� ǥ��.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTextBlock* Text_Value_Dump = nullptr;
	//	������ ���� ��ư. (Delegate�� �̿��� ���ο� â�� ���.)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Filter_Dump = nullptr;


#pragma endregion
	
private:
	//	���� ��� �� ���� ����Ʈ.
	TMap<ETabType, UTab_ListItem*> m_TabList;

	//	�������� �������� �� ȣ��. ���� â ǥ��.
	FOnItemClickEvent m_OnItemClick;
	//	������ ���� ��ư�� ������ �� ȣ��. ������ ���� â ǥ��.
	FOpenDumpFilterEvent m_OnDumpFilter;

	//	���� ������ EInventoryViewType.
	EInventoryViewType m_ViewType = EInventoryViewType::None;
	//	���� ������ ETabType.
	ETabType m_ActiveTabType = ETabType::All;

	//	���� ���õǾ� �ִ� �������� UID.
	int64 m_SelectItemUID = INDEX_NONE;

#pragma region View_Dump
	//	������ ���� ���õ� �������� UID ����Ʈ.
	TArray<int64> m_DumpSelectList;

#pragma endregion

#pragma region ListView_Data
	//	���� �ӽ÷� �����ϰ� �ִ� ������ ������ ����Ʈ.
	UPROPERTY() TMap<int64, UListItem_Item_Data*> ItemDataList;
	//	���� ����� �ʾ� �����ص� ������ Pool.
	UPROPERTY() TArray<UListItem_Item_Data*> PoolDataList;


#pragma endregion
};
