// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"

#include "CorpsMain.generated.h"

/**
 *
 */

class UImage;
class UListView;
class UCanvasPanel;
class UHorizontalBox;

UCLASS()
class UCorpsMain : public UObject
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void EventConnect() override;
	virtual void Release() override;

public:
	void CallbackCorpsChange(const bool InbResult);

private:
	void BuildDataByUser();

	//=========================================================================================================
	// tab widget.
	// char type tab.
	void CreateCharTypeTab();
	void CallbackChangeCharTab(const int32& InCharType);
	
	// corps tab.
	void CreateCorpsTabList();
	void OnTileViewListItemObjectSetEvent(UWLListItemBase* InItem);
	void CallbackCorpsTab(UWLFormation_Card_ListItem* InCorpsWidget);
	void CallbackCorpsTab(const int64& InCorpsUID);

	void SelectCorpsTab(const int64& InCorpsUID, const bool InbOn);
	void UpdateCorpsTabViewWidget(const int64& InCorpsUID);
	void UpdateAllCorpsTabViewWidget();

	//=========================================================================================================
	// popup widget.
	// combat power.
	void ShowCombatPower();

	// change name.
	void ShowChangeNamePopup();
	
	void CallbackChangeName(const FText& InText);
	
	// noti.
	void ShowDirtyPopup(FOnBasePopupMainEvent InMainButtonEvent);
	void HidePopup();
	void HideAllPopup();

	//=========================================================================================================
	// data.
	void ResetData();

	//=========================================================================================================
	// callback.
	// auto mount.
	void CallbackAutoMount();

	// all reset.
	void CallbackAllReset();

	// save.
	void CallbackFormationListSave();
	void CallbackSave();
	bool CheckLeaderEquip();

	// click hero card.
	void CallbackHeroCard(UWLHero* InHero);
	void CallbackArmyCard(UWLArmy* InArmy);
	void CallbackSlotCard(const int64& InTargetUID, const bool InbHero);
	void CallbackCard(const int64& InTargetUID, const bool InbHero);

	// click frame.
	void CallbackFrame(const EChar_type InCharType);

	// change hero.
	void CallbackEquipTarget(const int64& InTargetUID, const bool InbIsHero, const bool InbEquip);

	// info popup.
	void CallbackEquipHero(int32 InbHero, int64 InTargetUID);
	void CallbackUnEquipHero(int32 InbHero, int64 InTargetUID);

	// formaiton list popup.
	void CallbackEditCorps(const int64& InCorpsUID);

	// drag start and end.
	void CallbackDragState(bool InbDrag);
	void CallbackDragState(const bool InbIsHero, const bool InbDrag);

	// out.
	void CallBackPrev();
	void CallBackDirtyPrev();
	void CallBackExit();
	void CallBackDirtyExit();

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UCanvasPanel* CanvasPanel_FormationAdmin = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWLHero_List_Slot* List_Hero = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWLHeroInfoPopup_Form* frmP_HeroInfo = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWLHeroInfoPopup_Form* frmP_HeroInfo_Equip = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWLCombatPowerInfoPopup_Form* frmP_CombatPowerInfo = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UListView* ListView_DeckList = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWLCorps_Slot_Slot* Corps_Slot = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWLButton* Bt_Fold = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UCanvasPanel* CanvasPanel_Popup = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UImage* Img_Bg_Popup = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UWLFormation_ListPopup_Form* frmP_FormationList = nullptr;

private:
	TMap<EChar_type, UWLVerticalTab_ListItem*> m_CharTypeTabList;
	TMap<int32, UWLFormation_Card_ListItem_ItemData*> m_CorpsDataList;

private:
	int64 m_SelectCorpsUID = INDEX_NONE;
	EChar_type m_SelectCharType = EChar_type::None;
	TArray<int64> m_DirtyUIDList;
	int64 m_TempTargetUID = INDEX_NONE;
	bool m_SelectOn = false;
};