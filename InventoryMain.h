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
	* 처음 가방 UMG 진입 시 호출되는 함수.
	* 버튼과 가방 데이터 변경 시 호출되는 함수를 연결하고 UI에 아이템 정보를 띄워줌.
	*/
	void Init();

	/**
	* 이벤트와 연결한 함수들의 연결을 끊음.
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
	* view type에 맞춰 가방의 ui를 바꿔줌.
	* InViewType : 변경할 타입.
	*/
	void BuildDataByUser(const UInventoryMain::EInventoryViewType& InViewType);


#pragma endregion

#pragma region Tab
	/**
	* m_ViewType에 맞춰서 상단 탭의 내용을 바꿈.
	*/
	void SetTab();

	/**
	* 탭 바꿈. > 강화, 분해 등 다른 창에서 호출.
	* InTabType : 변경할 타입.
	*/
	void ChangeTab(ETabType InTabType);

	/**
	* 탭의 활성화 비활성화 ui 변경.
	* InOffTabType : 기존에 켜져 있어서 비활성화 상태로 만들 탭의 타입.
	* InOnTabType : 새롭게 활성화 상태로 만들 탭의 타입.
	*/
	void ChangeTabUI(const ETabType& InOffTabType, const ETabType& InOnTabType);


#pragma endregion

#pragma region Count Text
	/**
	* 현재 소지하고 있는 아이템의 갯수 표시.
	*/
	void SetCountText();

	/**
	* 현재 버리기 위해 선택한 아이템의 갯수 표시.
	*/
	void SetDumpCountText();


#pragma endregion

#pragma region Filter
	/**
	* m_ActiveTabType에 맞춰서 띄울 수 있는지 확인.
	* InItem : 확인할 타겟 Data.
	*/
	bool FilterItem(UItemData* InItem);


#pragma endregion

#pragma region Item
	/**
	* listview 위젯에 저장된 데이터들을 Pool에 저장한 후 지워줌.
	*/
	void ClearListItems();

	/**
	* 가방 데이터가 변경 될 시 호출되는 함수.
	* 모든 아이템 UI와 아이템 갯수 표시가 갱신됨.
	*/
	UFUNCTION() void UpdateInventory();

	/**
	* View_Dump에 진입 시 호출.
	* 모든 아이템 UI와 아이템 갯수 표시가 갱신됨.
	*/
	void UpdateDump();

	/**
	* InItemUID를 이용해 해당 ItemUID를 가진 UI만 갱신 시켜줌.
	* InItemUID : 타겟 아이템의 UID.
	*/
	void UpdateItem(const int64& InItemUID);

	/**
	* 소지하고 있는 모든 아이템의 UI가 갱신됨.
	*/
	void UpdateAllItem();

	/**
	* 아이템 UI를 새로 추가함.
	* InReddotSystem : 아이템이 새롭게 추가되었을 경우 레드닷을 띄워줘야 함.
	* InItemData : 새로 추가될 아이템 Data.
	*/
	void AddItem(UReddotSystem* InReddotSystem, UItemData* InItemData);

	/**
	* 아이템 갯수가 변경되거나 탭을 변경 했을 시 비어있는 칸의 갯수를 새로 갱신 시켜줌.
	*/
	void SyncEmptyItem();

	/**
	* 아이템을 선택했을 시 이펙트 변경.
	* InItemUID : 이펙트를 변경할 아이템의 UID.
	* InbSelect : 활성화, 비활성화 여부.
	*/
	void SelectItem(const int64& InItemUID, const bool InbSelect);


#pragma endregion

#pragma region ListView
	/**
	* 아이템 UI를 선택했을 시 호출되는 함수.
	* m_ViewType에 맞춰서 함수를 호출함.
	* InItemObject : 선택된 아이템의 Data.
	*/
	void Callback_ItemSelect(UObject* InItemObject);

	/**
	* InItemUID: 선택된 아이템의 UID.
	*/
	void Callback_ItemSelect_ViewEquip(const int64& InItemUID);
	void Callback_ItemSelect_ViewDump(const int64& InItemUID);


#pragma endregion

#pragma region AutoEquip
	/**
	* 자동 장착을 눌렀을 때 view type에 맞춰서 다른 함수를 호출함.
	*/
	void OnClickAutoEquip();


#pragma endregion

#pragma region Button_Event
private:
	/**
	* 버튼을 눌렀을 시 호출되는 함수.
	*/
	UFUNCTION() void OnClicked_Bt_Auto();
	UFUNCTION() void OnClicked_Bt_Dump();
	UFUNCTION() void OnClicked_Bt_Close_Dump();
	UFUNCTION() void OnClicked_Bt_Filter_Dump();


#pragma endregion

protected:
	//	m_ViewType에 따라 바꿔줘야 하는 위젯.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWidgetSwitcher* WidgetSwitcher_Popup = nullptr;
	//	m_ViewType에 따라 아이템 Data가 들어갈 UTileView.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTileView* TileView_Item = nullptr;
	//	m_ViewType에 따라 아이템 Data가 들어갈 UTileView.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTileView* TileView_Dump = nullptr;
	//	현재 소지하고 있는 아이템의 갯수를 표시.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTextBlock* Text_Value = nullptr;
	//	장비 자동장착 버튼.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Auto = nullptr;


#pragma region View_Equip
	//	버리기 버튼. (EInventoryViewType을 Equip에서 Dump로 변경.)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Dump = nullptr;


#pragma endregion

#pragma region View_Dump
	//	버리기 닫기 버튼. (EInventoryViewType을 Dump에서 Equip으로 변경.)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Close_Dump = nullptr;
	//	현재 버리려고 선택한 아이템의 갯수 표시.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTextBlock* Text_Value_Dump = nullptr;
	//	버리기 필터 버튼. (Delegate를 이용해 새로운 창을 띄움.)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Filter_Dump = nullptr;


#pragma endregion
	
private:
	//	가방 상단 탭 위젯 리스트.
	TMap<ETabType, UTab_ListItem*> m_TabList;

	//	아이템을 선택했을 시 호출. 상태 창 표시.
	FOnItemClickEvent m_OnItemClick;
	//	버리기 필터 버튼을 눌렀을 시 호출. 버리기 필터 창 표시.
	FOpenDumpFilterEvent m_OnDumpFilter;

	//	현재 가방의 EInventoryViewType.
	EInventoryViewType m_ViewType = EInventoryViewType::None;
	//	현재 가방의 ETabType.
	ETabType m_ActiveTabType = ETabType::All;

	//	현재 선택되어 있는 아이템의 UID.
	int64 m_SelectItemUID = INDEX_NONE;

#pragma region View_Dump
	//	버리기 위해 선택된 아이템의 UID 리스트.
	TArray<int64> m_DumpSelectList;

#pragma endregion

#pragma region ListView_Data
	//	현재 임시로 저장하고 있는 아이템 데이터 리스트.
	UPROPERTY() TMap<int64, UListItem_Item_Data*> ItemDataList;
	//	현재 띄우지 않아 저장해둔 데이터 Pool.
	UPROPERTY() TArray<UListItem_Item_Data*> PoolDataList;


#pragma endregion
};
