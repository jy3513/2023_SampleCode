// Fill out your copyright notice in the Description page of Project Settings.

#include "CorpsMain.h"

#include "Components/ListView.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"

//#include "FunctionLibrary/WLFunctionLibrary_System.h"
//#include "FunctionLibrary/WLFunctionLibrary_Contents.h"
//#include "FunctionLibrary/WLFunctionLibrary_Network.h"
//
//#include "Network/flatBuffer/Compile/TCPProtocol_generated.h"

void UCorpsMain::Init()
{
#pragma region InitWidget
    CHECK_INVALID_QUIT(List_Hero);
    CHECK_INVALID_QUIT(frmP_HeroInfo);
    CHECK_INVALID_QUIT(frmP_HeroInfo_Equip);
    CHECK_INVALID_QUIT(frmP_CombatPowerInfo);
    CHECK_INVALID_QUIT(Corps_Slot);
    CHECK_INVALID_QUIT(frmP_FormationList);
    List_Hero->InitWidget();
    List_Hero->SetListType(UWLHero_List_Slot::EHeroListType::Corps);
    List_Hero->ActiveAutoMount(false);
    List_Hero->BuildDataByUser();
    frmP_HeroInfo->InitWidget();
    frmP_HeroInfo_Equip->InitWidget();
    frmP_CombatPowerInfo->InitWidget();
    Corps_Slot->InitWidget();
    frmP_FormationList->InitWidget();

    EventConnect();

    BuildDataByUser();

#pragma endregion
}

void UCorpsMain::EventConnect()
{
    CHECK_INVALID_QUIT(Bt_Fold);
    CHECK_INVALID_QUIT(List_Hero);
    CHECK_INVALID_QUIT(frmP_HeroInfo);
    CHECK_INVALID_QUIT(frmP_HeroInfo_Equip);
    CHECK_INVALID_QUIT(frmP_CombatPowerInfo);
    CHECK_INVALID_QUIT(Corps_Slot);
    CHECK_INVALID_QUIT(frmP_FormationList);

    // corps slot.
    {
        // 종합 전투력 버튼.
        Corps_Slot->SetOnCombatPowerEvent(UWLCorps_Slot_Slot::FOnClickEvent::CreateUObject(this, &UCorpsMain::ShowCombatPower));
        // 이름 변경 버튼.
        Corps_Slot->SetOnNameEvent(UWLCorps_Slot_Slot::FOnClickEvent::CreateUObject(this, &UCorpsMain::ShowChangeNamePopup));
        // 자동 장착 버튼.
        Corps_Slot->SetOnAutoEvent(UWLCorps_Slot_Slot::FOnClickEvent::CreateUObject(this, &UCorpsMain::CallbackAutoMount));
        // 해제 버튼.
        Corps_Slot->SetOnClearEvent(UWLCorps_Slot_Slot::FOnClickEvent::CreateUObject(this, &UCorpsMain::CallbackAllReset));
        // 저장 버튼.
        Corps_Slot->SetOnSaveEvent(UWLCorps_Slot_Slot::FOnClickEvent::CreateUObject(this, &UCorpsMain::CallbackSave));
        // 드래그로 영웅 정보가 변경 되었음.
        Corps_Slot->SetOnEquipEvent(UWLCorps_Slot_Slot::FOnEquipEvent::CreateUObject(this, &UCorpsMain::CallbackEquipTarget));
        // 영웅 정보 팝업.
        Corps_Slot->SetOnInfoEvent(UWLCorps_Slot_Slot::FOnInfoEvent::CreateUObject(this, &UCorpsMain::CallbackSlotCard));
        // 선택 상태일 때 프레임 클릭.
        Corps_Slot->SetOnFrameClickEvent(UWLHeroCard_Frame_Type_1_ListItem::FOnFrameClickEvent::CreateUObject(this, &UCorpsMain::CallbackFrame));
    }
    // list hero.
    {
        // 영웅 정보 팝업.
        List_Hero->SetOnClickCardEvent(UWLHero_List_Slot::FOnClickCardEvent::CreateUObject(this, &UCorpsMain::CallbackHeroCard));
        List_Hero->SetOnClickArmyCardEvent(UWLHero_List_Slot::FOnClickArmyCardEvent::CreateUObject(this, &UCorpsMain::CallbackArmyCard));
        // 영웅 슬롯의 드래그 상태를 변경해줌.
        List_Hero->SetOnDragEvent(UWLHero_List_Slot::FOnDragEvent::CreateUObject(this, &UCorpsMain::CallbackDragState));
    }
    // hero info.
    {
        // 영웅 장착.
        frmP_HeroInfo->SetOnUnEquipEvent(UWLHeroInfoPopup_Form::FOnUnEquipEvent::CreateUObject(this, &UCorpsMain::CallbackEquipHero));
        frmP_HeroInfo->SetListType(UWLHeroInfoPopup_Form::EHeroListType::Corps);
    }
    // hero equip info.
    {
        // 영웅 해제.
        frmP_HeroInfo_Equip->SetOnUnEquipEvent(UWLHeroInfoPopup_Form::FOnUnEquipEvent::CreateUObject(this, &UCorpsMain::CallbackUnEquipHero));
        frmP_HeroInfo_Equip->SetListType(UWLHeroInfoPopup_Form::EHeroListType::Corps);
    }
    // foramtion list.
    {
        // 펼쳐보기에서 부대 편집 버튼.
        frmP_FormationList->SetOnClickEvent(FOnClickEvent::CreateUObject(this, &UCorpsMain::CallbackEditCorps));
        // 일괄 저장.
        frmP_FormationList->SetOnAllSaveEvent(FOnChangeNameEvent::CreateUObject(this, &UCorpsMain::CallbackFormationListSave));
    }
}

void UCorpsMain::Release()
{

}

void UCorpsMain::CallbackCorpsChange(const bool InbResult)
{
    CHECK_INVALID_QUIT(Corps_Slot);
    if (InbResult)
    {
        // success.
        m_DirtyUIDList.Empty();
        Corps_Slot->SetActiveSaveUI(false);
        ShowNoticePopup(static_cast<int32>(ESystemMsgDefine::UISysMsg_Saved));
    }
    else
    {
        // fail.
        // 22.11.10. 현재 실패에 관한 예외처리가 기획되지 않음.
    }
}
void UCorpsMain::BuildDataByUser()
{
    // reset.
    ResetData();
    HideAllPopup();

    // create widget.
    CreateCharTypeTab();
    
    CreateCorpsTabList();
    // init.
    if (m_CorpsDataList.Num() > 0)
    {
        CallbackCorpsTab(m_CorpsDataList.begin().Key());
    }
    
    CallbackChangeCharTab(static_cast<int32>(m_SelectCharType));
}

void UCorpsMain::CreateCharTypeTab()
{
    UWLTable* Table = UWLFunctionLibrary_System::GetTable(this);

    if (Table == nullptr || Table->IsValidLowLevel() == false)
    {
        UE_LOG();
        return;
    }

    m_CharTypeTabList.Emplace(EChar_type::Hero, Cast<UWLVerticalTab_ListItem>(GetWidgetFromName(TEXT("ListItem_Tab_Hero"))));
    m_CharTypeTabList.Emplace(EChar_type::Soldier, Cast<UWLVerticalTab_ListItem>(GetWidgetFromName(TEXT("ListItem_Tab_Soldier"))));
    m_CharTypeTabList.Emplace(EChar_type::Spirit, Cast<UWLVerticalTab_ListItem>(GetWidgetFromName(TEXT("ListItem_Tab_Elemental"))));
    m_CharTypeTabList.Emplace(EChar_type::SiegeWeapon, Cast<UWLVerticalTab_ListItem>(GetWidgetFromName(TEXT("ListItem_Tab_SiegeWeapon"))));
    
    m_SelectCharType = EChar_type::Hero;

    for (const TPair<EChar_type, UWLVerticalTab_ListItem*>& TabInfo : m_CharTypeTabList)
    {
        if (nullptr == TabInfo.Value)
            continue;
        TabInfo.Value->InitWidget();
        TabInfo.Value->SetTabIndex(static_cast<int32>(TabInfo.Key));
        TabInfo.Value->SetOnTabClickEvent(UWLListItemTabBase::FOnClickTab::CreateUObject(this, &UCorpsMain::CallbackChangeCharTab));
        FText Content = FText();
        switch (TabInfo.Key)
        {
        case EChar_type::Hero: Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CharType_Hero), Content); break;
        case EChar_type::Soldier: Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CharType_Soldier), Content); break;
        case EChar_type::Spirit: Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CharType_Spirit), Content); break;
        case EChar_type::SiegeWeapon: Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CharType_SiegeWeapon), Content); break;
        }

        TabInfo.Value->SetTabTitleContent(Content);
    }
}

void UCorpsMain::CallbackChangeCharTab(const int32& InCharType)
{
    CHECK_INVALID_QUIT(List_Hero);
    //=========================================================================================================
    if (m_CharTypeTabList.Contains(m_SelectCharType) && m_CharTypeTabList[m_SelectCharType] != nullptr && m_CharTypeTabList[m_SelectCharType]->IsValidLowLevel())
    {
        m_CharTypeTabList[m_SelectCharType]->SetActive(false);
    }
    const EChar_type CharType = static_cast<EChar_type>(InCharType);
    if (m_CharTypeTabList.Contains(CharType) && m_CharTypeTabList[CharType] != nullptr && m_CharTypeTabList[CharType]->IsValidLowLevel())
    {
        m_SelectCharType = CharType;
        m_CharTypeTabList[CharType]->SetActive(true);
    }
    //=========================================================================================================
    List_Hero->SettingHeroCharType(InCharType);
}

void UCorpsMain::CreateCorpsTabList()
{
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    TArray<int64> LeaderUIDList;
    TArray<UWLCorpsInfo*> CorpsList;
    CorpsSystem->GetCorpsInfoList(CorpsList);
    for (UWLCorpsInfo* CorpsInfo : CorpsList)
    {
        if (CorpsInfo == nullptr || CorpsInfo->IsValidLowLevel() == false)
            continue;
        UWLFormation_Card_ListItem_ItemData* ItemData = NewObject<UWLFormation_Card_ListItem_ItemData>(this);
        if (ItemData == nullptr)
        {
            continue;
        }
        if (CorpsInfo->GetName().IsEmpty())
        {
            CorpsInfo->SetName(FText::FromString(FString::Printf(TEXT("squad %d"), ListView_DeckList->GetNumItems() + 1)));
        }
        ItemData->m_Index = CorpsInfo->GetUID();
        ItemData->m_CorpsInfo = CorpsInfo;
        ItemData->OnSetDataEvent = UWLListItemBase::FOnSetDataEvent::CreateUObject(this, &UCorpsMain::OnTileViewListItemObjectSetEvent);
        LeaderUIDList.Emplace(CorpsInfo->GetOfficerUID());
        m_CorpsDataList.Emplace(CorpsInfo->GetUID(), ItemData);
        ListView_DeckList->AddItem(ItemData);
    }
    for (const int64& UID : LeaderUIDList)
    {
        if (0 >= UID)
            continue;
        List_Hero->SetLeaderCardIcon(UID, true);
    }
}

void UCorpsMain::OnTileViewListItemObjectSetEvent(UWLListItemBase* InItem)
{
    UWLFormation_Card_ListItem* CardWidget = Cast<UWLFormation_Card_ListItem>(InItem);
    CHECK_INVALID_QUIT(CardWidget);
    //=========================================================================================================
    // set event.
    if (false == CardWidget->IsOnClickCheckValid())
        CardWidget->SetOnClickEvent(UWLFormation_Card_ListItem::FOnClickEvent::CreateUObject(this, &UCorpsMain::CallbackCorpsTab));
}

void UCorpsMain::CallbackCorpsTab(UWLFormation_Card_ListItem* InCorpsWidget)
{
    CHECK_INVALID_QUIT(InCorpsWidget);
    CallbackCorpsTab(InCorpsWidget->GetUID());
}

void UCorpsMain::CallbackCorpsTab(const int64& InCorpsUID)
{
    if (m_SelectCorpsUID == InCorpsUID)
    {
        return;
    }
    CHECK_INVALID_QUIT(Corps_Slot);
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    const UWLCorpsInfo* CorpsInfo = CorpsSystem->GetCorpsInfo(InCorpsUID);
    CHECK_INVALID_QUIT(CorpsInfo);
    //=========================================================================================================
    // select corps card.
    SelectCorpsTab(m_SelectCorpsUID, false);
    m_SelectCorpsUID = InCorpsUID;
    SelectCorpsTab(m_SelectCorpsUID, true);
    // update data.
    Corps_Slot->BuildDataByUser(CorpsInfo);
    // all collapsed.
    HideAllPopup();
    // off effect.
    CallbackDragState(false, false);
}

void UCorpsMain::SelectCorpsTab(const int64& InCorpsUID, const bool InbOn)
{
    if (m_CorpsDataList.Contains(InCorpsUID) == false)
    {
        return;
    }
    CHECK_INVALID_QUIT(m_CorpsDataList[InCorpsUID]);
    m_CorpsDataList[InCorpsUID]->m_State = InbOn ? (int32)UWLFormation_Card_ListItem::ECardState::None;
    UpdateCorpsTabViewWidget(InCorpsUID);
}

void UCorpsMain::UpdateCorpsTabViewWidget(const int64& InCorpsUID)
{
    if (m_CorpsDataList.Contains(InCorpsUID) == false)
    {
        return;
    }
    CHECK_INVALID_QUIT(ListView_DeckList);
    CHECK_INVALID_QUIT(m_CorpsDataList[InCorpsUID]);
    UWLFormation_Card_ListItem* ViewWidget = Cast<UWLFormation_Card_ListItem>(ListView_DeckList->GetEntryWidgetFromItem(m_CorpsDataList[InCorpsUID]));
    CHECK_INVALID_QUIT(ViewWidget);
    ViewWidget->UpdateCard();
}

void UCorpsMain::UpdateAllCorpsTabViewWidget()
{
    CHECK_INVALID_QUIT(ListView_DeckList);
    for (const TPair<int32, UWLFormation_Card_ListItem_ItemData*>& CorpData : m_CorpsDataList)
    {
        if (CorpData.Value == nullptr || CorpData.Value->IsValidLowLevel() == false)
        {
            continue;
        }
        UWLFormation_Card_ListItem* ViewWidget = Cast<UWLFormation_Card_ListItem>(ListView_DeckList->GetEntryWidgetFromItem(CorpData.Value));
        if (ViewWidget == nullptr || ViewWidget->IsValidLowLevel() == false)
        {
            continue;
        }
        ViewWidget->UpdateCard();
    }
}
void UCorpsMain::ShowCombatPower()
{
    CHECK_INVALID_QUIT(frmP_CombatPowerInfo);
    UWLGameUISystem* GameUISystem = UWLFunctionLibrary_System::GetGameUISystem(this);
    CHECK_INVALID_QUIT(GameUISystem);
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    const UWLCorpsInfo* CorpsInfo = CorpsSystem->GetCorpsInfo(m_SelectCorpsUID);
    CHECK_INVALID_QUIT(CorpsInfo);
    frmP_CombatPowerInfo->SetCombatPowerInfo(CorpsInfo);
    Owner->ShowChildWidget(frmP_CombatPowerInfo, ESlateVisibility::SelfHitTestInvisible);
}
void UCorpsMain::ShowChangeNamePopup()
{
    CHECK_INVALID_QUIT(CanvasPanel_Popup);
    CHECK_INVALID_QUIT(Img_Bg_Popup);
    UWLGameUISystem* GameUISystem = UWLFunctionLibrary_System::GetGameUISystem(this);
    CHECK_INVALID_QUIT(GameUISystem);
    UWLTable* Table = UWLFunctionLibrary_System::GetTable(this);
    CHECK_INVALID_QUIT(Table);
    FText Content = FText();
    FWLBasePopup_FormArg Args;
    Args.TextType = EBasePopupTextType::TypeB;
    Args.ButtonType = EBasePopupButtonType::TwoButtons;
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CorpsNameChange_Title), Args.Title);
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CorpsNameChange_Content), Args.MainText);
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CorpsNameChange_Limit), Args.SubText);
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_Confirm), Args.MainButtonText);
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_Cancel), Args.SubButtonText);
    Args.OnSubEvent = FOnBasePopupSubEvent::CreateUObject(this, &UCorpsMain::HidePopup);
    Args.OnInputTextEvent = FOnBasePopupInputTextEvent::CreateUObject(this, &UCorpsMain::CallbackChangeName);
    GameUISystem->Show(EUINameType::frmP_Popup, &Args);
    CanvasPanel_Popup->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    Img_Bg_Popup->SetVisibility(ESlateVisibility::Visible);
    GameUISystem->Hide(EUINameType::frmP_OfficerSkillInfo);
    GameUISystem->Hide(EUINameType::frmP_PictorialBook_Skill);
}
void UCorpsMain::CallbackChangeName(const FText& InText)
{
    CHECK_INVALID_QUIT(Corps_Slot);
    //=========================================================================================================
    // name check.
    if (1 > InText.ToString().Len())
    {
        ShowNoticePopup(static_cast<int32>(ESystemMsgDefine::UISysMsg_PleaseEnterLeastOneCharacter));
        HidePopup();
        return;
    }
    //=========================================================================================================
    // set data.
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    const UWLCorpsInfo* CorpsInfo = CorpsSystem->GetCorpsInfo(m_SelectCorpsUID);
    CHECK_INVALID_QUIT(CorpsInfo);
    CorpsSystem->SetCorpsInfo(CorpsInfo->GetUID(), CorpsInfo->GetOfficerUID(), CorpsInfo->GetArmyUID(), InText);
    m_DirtyUIDList.Emplace(m_SelectCorpsUID);
    //=========================================================================================================
    // set ui.
    Corps_Slot->ChangeName(InText);
    Corps_Slot->SetActiveSaveUI(true);
    UpdateCorpsTabViewWidget(m_SelectCorpsUID);
    HidePopup();
}

void UCorpsMain::ShowDirtyPopup(FOnBasePopupMainEvent InMainButtonEvent)
{
    CHECK_INVALID_QUIT(CanvasPanel_Popup);
    CHECK_INVALID_QUIT(Img_Bg_Popup);
    UWLTable* Table = UWLFunctionLibrary_System::GetTable(this);
    CHECK_INVALID_QUIT(Table);
    UWLGameUISystem* GameUISystem = UWLFunctionLibrary_System::GetGameUISystem(this);
    CHECK_INVALID_QUIT(GameUISystem);
    FWLBasePopup_FormArg Args;
    Args.TextType = EBasePopupTextType::TypeA;
    Args.ButtonType = EBasePopupButtonType::TwoButtons;
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_Confirm), Args.MainButtonText);
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_Cancel), Args.SubButtonText);
    //===================================
    // title.
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CorpsChangeNotSaveOut_Title), Args.Title);
    //===================================
    // content.
    Table->GetUiMsgStringDataAt(static_cast<int32>(EUIMsgDefine::UIMsg_CorpsChangeNotSaveOut_Content), Args.MainText);
    Args.MainText = FText::FromString(Args.MainText.ToString().Replace(TEXT("\\n"), TEXT("\n")));
    //===================================
    // event.
    Args.OnMainEvent = InMainButtonEvent;
    Args.OnSubEvent = FOnBasePopupSubEvent::CreateUObject(this, &UCorpsMain::HidePopup);
    //===================================
    // show.
    GameUISystem->Show(EUINameType::frmP_Popup, &Args);
    CanvasPanel_Popup->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    Img_Bg_Popup->SetVisibility(ESlateVisibility::Visible);
}

void UCorpsMain::HidePopup()
{
    CHECK_INVALID_QUIT(CanvasPanel_Popup);
    UWLGameUISystem* GameUISystem = UWLFunctionLibrary_System::GetGameUISystem(this);

    CHECK_INVALID_QUIT(GameUISystem);
    CanvasPanel_Popup->SetVisibility(ESlateVisibility::Collapsed);
    GameUISystem->Hide(EUINameType::frmP_OfficerSkillInfo);
    GameUISystem->Hide(EUINameType::frmP_PictorialBook_Skill);
}

void UCorpsMain::HideAllPopup()
{
    CHECK_INVALID_QUIT(frmP_HeroInfo);
    CHECK_INVALID_QUIT(frmP_HeroInfo_Equip);
    CHECK_INVALID_QUIT(frmP_CombatPowerInfo);
    CHECK_INVALID_QUIT(CanvasPanel_Popup);
    UWLGameUISystem* GameUISystem = UWLFunctionLibrary_System::GetGameUISystem(this);
    CHECK_INVALID_QUIT(GameUISystem);
    frmP_HeroInfo->SetVisibility(ESlateVisibility::Collapsed);
    frmP_HeroInfo_Equip->SetVisibility(ESlateVisibility::Collapsed);
    frmP_CombatPowerInfo->SetVisibility(ESlateVisibility::Collapsed);
    CanvasPanel_Popup->SetVisibility(ESlateVisibility::Collapsed);
    GameUISystem->Hide(EUINameType::frmP_OfficerSkillInfo);
    GameUISystem->Hide(EUINameType::frmP_PictorialBook_Skill);
}

void UCorpsMain::ResetData()
{
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    CorpsSystem->Reset();
}

void UCorpsMain::CallbackAutoMount()
{
    CHECK_INVALID_QUIT(Corps_Slot);
    CHECK_INVALID_QUIT(List_Hero);
    //=========================================================================================================
    // set data.
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    const UWLCorpsInfo* CorpsInfo = CorpsSystem->GetAutoMountInfo(m_SelectCorpsUID);
    CHECK_INVALID_QUIT(CorpsInfo);
    CorpsSystem->SetCorpsInfo(m_SelectCorpsUID, CorpsInfo->GetOfficerUID(), CorpsInfo->GetArmyUID(), CorpsInfo->GetName());
    m_DirtyUIDList.Emplace(m_SelectCorpsUID);


    //================================================
   // set ui.
    ECorpsSlotType SlotType = ECorpsSlotType::None;
    if (CorpsInfo->GetOfficerUID() > 0 && CorpsInfo->GetArmyUID() <= 0)
    {
        SlotType = ECorpsSlotType::Hero;
    }
    else if (CorpsInfo->GetOfficerUID() <= 0 && CorpsInfo->GetArmyUID() > 0)
    {
        SlotType = ECorpsSlotType::Army;
    }
    Corps_Slot->Equip(CorpsInfo, SlotType);
    Corps_Slot->SetActiveSaveUI(true);
    List_Hero->UpdateCountCharacterLeft();
    UpdateCorpsTabViewWidget(m_SelectCorpsUID);
    HideAllPopup();
}

void UCorpsMain::CallbackAllReset()
{
    CHECK_INVALID_QUIT(Corps_Slot);
    CHECK_INVALID_QUIT(List_Hero);
    //=========================================================================================================
    // set data.
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    const UWLCorpsInfo* CorpsInfo = CorpsSystem->GetCorpsInfo(m_SelectCorpsUID);
    CHECK_INVALID_QUIT(CorpsInfo);
    CorpsSystem->SetCorpsInfo(m_SelectCorpsUID, 0, 0, CorpsInfo->GetName());
    m_DirtyUIDList.Emplace(m_SelectCorpsUID);
    //=========================================================================================================
    // set ui.
    Corps_Slot->Equip(CorpsInfo, ECorpsSlotType::None);
    Corps_Slot->SetActiveSaveUI(true);
    List_Hero->UpdateCountCharacterLeft();
    UpdateCorpsTabViewWidget(m_SelectCorpsUID);
    HideAllPopup();
}

void UCorpsMain::CallbackFormationListSave()
{
    if (m_DirtyUIDList.Num() <= 0)
    {
        ShowNoticePopup(static_cast<int32>(ESystemMsgDefine::UISysMsg_NoChangesCanBeSaved));
        return;
    }
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    TArray<int64> ChangeCorpsUIDList;
    if (CorpsSystem->GetChangeCorpsUID(m_DirtyUIDList, ChangeCorpsUIDList) == false)


    {
        ShowNoticePopup(static_cast<int32>(ESystemMsgDefine::UISysMsg_NoChangesCanBeSaved));
        return;
    }
    if (CheckLeaderEquip() == false)
    {
        // 22.11.05. jyqwerty. todo : 노티 추가 예정
        //ShowNoticePopup();
        return;
    }
    CorpsSystem->Save(ChangeCorpsUIDList);
}

void UCorpsMain::CallbackSave()
{
    CallbackFormationListSave();
    HideAllPopup();
}

bool UCorpsMain::CheckLeaderEquip()
{
    // 수정 된 부대 중 하나라도 영웅이 장착되지 않으면 저장하지 못함.
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT_RETURN_TYPE(GameInstance, false);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT_RETURN_TYPE(CorpsSystem, false);
    TArray<UWLCorpsInfo*> CorpsList;
    CorpsSystem->GetCorpsInfoList(CorpsList);
    for (const UWLCorpsInfo* CorpsInfo : CorpsList)
    {
        CHECK_INVALID_CONTINUE(CorpsInfo);
        if (m_DirtyUIDList.Contains(CorpsInfo->GetUID()) == false)
        {
            continue;
        }
        if (CorpsInfo->GetOfficerUID() <= 0 && CorpsInfo->GetArmyUID() > 0)
        {
            return false;
        }
    }
    return true;
}

void UCorpsMain::CallbackHeroCard(UWLHero* InHero)
{
    CHECK_INVALID_QUIT(InHero);
    CallbackCard(InHero->GetUID(), true);
}

void UCorpsMain::CallbackArmyCard(UWLArmy* InArmy)
{
    CHECK_INVALID_QUIT(InArmy);
    CallbackCard(InArmy->GetArmyDataInfo().UID, false);
}

void UCorpsMain::CallbackSlotCard(const int64& InTargetUID, const bool InbHero)
{
    if (m_SelectOn)
    {
        // 슬롯에 카드 이펙트가 켜져 있으면 정보 창이 아닌 교체.
        if (InbHero)
        {
            CallbackFrame(EChar_type::Hero);
        }
        else
        {
            CallbackFrame(EChar_type::Soldier);
        }
    }
    else
    {
        CallbackCard(InTargetUID, InbHero);
    }
}

void UCorpsMain::CallbackCard(const int64& InTargetUID, const bool InbHero)
{
    if (InTargetUID <= 0)
    {
        return;
    }
    UWLGameUISystem* GameUISystem = UWLFunctionLibrary_System::GetGameUISystem(this);
    CHECK_INVALID_QUIT(GameUISystem);
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    const UWLCorpsInfo* CorpsInfo = CorpsSystem->GetCorpsInfo(m_SelectCorpsUID);
    CHECK_INVALID_QUIT(CorpsInfo);
    // 먼저 꺼줘야 됨.
    // off select.
    CallbackDragState(false, false);
    //
    TArray<FArmyUnitSlotInfo> HeroList;
    if (CorpsInfo->GetOfficerUID() == InTargetUID || CorpsInfo->GetArmyUID() == InTargetUID)
    {
        CHECK_INVALID_QUIT(frmP_HeroInfo_Equip);
        HeroList.Emplace(FArmyUnitSlotInfo(1, CorpsInfo->GetOfficerUID()));
        HeroList.Emplace(FArmyUnitSlotInfo(2, CorpsInfo->GetArmyUID()));
        frmP_HeroInfo_Equip->SetSquadHeroList(HeroList);
        frmP_HeroInfo_Equip->SetLeaderHeroUID(CorpsInfo->GetOfficerUID());
        if (InbHero)
        {
            frmP_HeroInfo_Equip->SetHeroInfo(InTargetUID, true);
        }
        else
        {
            frmP_HeroInfo_Equip->SetArmyInfo(InTargetUID, true);
        }
        GameUISystem->ShowChildWidget(frmP_HeroInfo_Equip, ESlateVisibility::SelfHitTestInvisible);
    }
    else
    {
        CHECK_INVALID_QUIT(List_Hero);
        CHECK_INVALID_QUIT(frmP_HeroInfo);
        TArray<TPair<int64, int32>> TargetDataList;
        List_Hero->GetShowCardDataList(TargetDataList);
        for (const TPair<int64, int32>& TargetData : TargetDataList)
        {
            HeroList.Emplace(FArmyUnitSlotInfo(HeroList.Num() + 1, TargetData.Key));
        }
        frmP_HeroInfo->SetSquadHeroList(HeroList);
        if (InbHero)
        {
            frmP_HeroInfo->SetHeroInfo(InTargetUID, false);
            if (CorpsInfo->GetOfficerUID() > 0)
            {
                frmP_HeroInfo->SetStateChangeOnlyText(true);
            }
            else
            {
                frmP_HeroInfo->SetStateChange(false);
            }
        }
        else
        {
            frmP_HeroInfo->SetArmyInfo(InTargetUID, false);
            if (CorpsInfo->GetArmyUID() > 0)
            {
                frmP_HeroInfo->SetStateChangeOnlyText(true);
            }
            else
            {
                frmP_HeroInfo->SetStateChange(false);
            }
        }
        GameUISystem->ShowChildWidget(frmP_HeroInfo, ESlateVisibility::SelfHitTestInvisible);
    }
}
void UCorpsMain::CallbackFrame(const EChar_type InCharType)
{
    if (InCharType == EChar_type::None)
    {
        return;
    }
    CHECK_INVALID_QUIT(Corps_Slot);
    // 현재 오른쪽 타입과 선택한 프레임의 타입이 다를 경우 교체 불가능한 플로우라 이렇게 예외처리 해둠. 추후 변경 될 경우 수정 필요.
    if (m_SelectCharType == InCharType)
    {
        CallbackEquipTarget(m_TempTargetUID, (bool)(InCharType == EChar_type::Hero), true);
    }
    CallbackDragState(false, false);
    HideAllPopup();
}
void UCorpsMain::CallbackEquipTarget(const int64& InTargetUID, const bool InbIsHero, const bool InbEquip)
{
    CHECK_INVALID_QUIT(Corps_Slot);
    UWLGameInstance* GameInstance = UWLFunctionLibrary_System::GetGameInstance(this);
    CHECK_INVALID_QUIT(GameInstance);
    UWLCorpsSystem* CorpsSystem = GameInstance->GetCorpsSystem();
    CHECK_INVALID_QUIT(CorpsSystem);
    const UWLCorpsInfo* CorpsInfo = CorpsSystem->GetCorpsInfo(m_SelectCorpsUID);
    CHECK_INVALID_QUIT(CorpsInfo);
    if (InbEquip)
    {
        if (CorpsSystem->GetCountCharacterLeft(InTargetUID, CorpsSystem->GetCharacterID(InTargetUID, InbIsHero), CorpsInfo->GetUID()) <= 0)
        {
            HideAllPopup();
            return;
        }
        if (InbIsHero)
        {
            CorpsSystem->SetCorpsInfo(CorpsInfo->GetUID(), InTargetUID, CorpsInfo->GetArmyUID(), CorpsInfo->GetName());
            // set ui.
            Corps_Slot->Equip(CorpsInfo, ECorpsSlotType::Hero);
        }
        else
        {
            CorpsSystem->SetCorpsInfo(CorpsInfo->GetUID(), CorpsInfo->GetOfficerUID(), InTargetUID, CorpsInfo->GetName());
            // set ui.
            Corps_Slot->Equip(CorpsInfo, ECorpsSlotType::Army);
        }
    }
    else
    {
        if (CorpsInfo->GetOfficerUID() == InTargetUID)
        {
            CorpsSystem->SetCorpsInfo(CorpsInfo->GetUID(), 0, CorpsInfo->GetArmyUID(), CorpsInfo->GetName());
        }
        else
        {
            CorpsSystem->SetCorpsInfo(CorpsInfo->GetUID(), CorpsInfo->GetOfficerUID(), 0, CorpsInfo->GetName());
        }
        // set ui.
        Corps_Slot->BuildDataByUser(CorpsInfo);
    }

    //=========================================================================================================
    // set ui.
    m_DirtyUIDList.Emplace(m_SelectCorpsUID);
    UpdateCorpsTabViewWidget(m_SelectCorpsUID);
    Corps_Slot->SetActiveSaveUI(true);
    List_Hero->UpdateCountCharacterLeft();

    HideAllPopup();
}
void UCorpsMain::CallbackEquipHero(int32 InbHero, int64 InTargetUID)
{
    CHECK_INVALID_QUIT(frmP_HeroInfo_Equip);
    // 왼쪽 장착 목록 팝업이 꺼져 있으면 선택해야 바뀜......
    if (frmP_HeroInfo_Equip->GetVisibility() == ESlateVisibility::Collapsed)
    {
        m_TempTargetUID = InTargetUID;
        CallbackDragState((bool)(InbHero), true);
    }
    else
    {
        CallbackEquipTarget(InTargetUID, (bool)(InbHero), true);
        CallbackDragState(false, false);
        HideAllPopup();
    }
}
void UCorpsMain::CallbackUnEquipHero(int32 InbHero, int64 InTargetUID)
{
    m_TempTargetUID = INDEX_NONE;
    CallbackEquipTarget(InTargetUID, (bool)(InbHero), false);
}
void UCorpsMain::CallbackEditCorps(const int64& InCorpsUID)
{
    CHECK_INVALID_QUIT(CanvasPanel_FormationAdmin);
    CHECK_INVALID_QUIT(CanvasPanel_Popup);
    CHECK_INVALID_QUIT(frmP_FormationList);
    CHECK_INVALID_QUIT(Corps_Slot);
    CHECK_INVALID_QUIT(List_Hero);
    //=========================================================================================================
    // update ui.
    if (m_SelectCorpsUID != InCorpsUID)
    {
        SelectCorpsTab(m_SelectCorpsUID, false);
    }
    const int64 TempCorpsUID = InCorpsUID;
    m_SelectCorpsUID = INDEX_NONE;
    const bool IsChange = frmP_FormationList->GetNeedSaveArmyUnitList(m_DirtyUIDList);
    Corps_Slot->SetActiveSaveUI(IsChange);
    List_Hero->UpdateCountCharacterLeft();
    frmP_FormationList->SelectMode(false);
    CallbackCorpsTab(TempCorpsUID);
    UpdateAllCorpsTabViewWidget();
    //=========================================================================================================
    CanvasPanel_FormationAdmin->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    CanvasPanel_Popup->SetVisibility(ESlateVisibility::Collapsed);
    frmP_FormationList->SetVisibility(ESlateVisibility::Collapsed);
}

void UCorpsMain::CallbackDragState(bool InbDrag)
{
    CallbackDragState((bool)(EChar_type::Hero == m_SelectCharType), InbDrag);
}

void UCorpsMain::CallbackDragState(const bool InbIsHero, const bool InbDrag)
{
    CHECK_INVALID_QUIT(Corps_Slot);
    m_SelectOn = InbDrag;
    if (m_SelectOn)
    {
        Corps_Slot->SetSelectFrameUI((bool)(InbIsHero) ? ECorpsSlotType::Hero : ECorpsSlotType::Army);
    }
    else
    {
        m_TempTargetUID = INDEX_NONE;
        Corps_Slot->SetSelectFrameUI(ECorpsSlotType::None);
    }
}

void UCorpsMain::CallBackPrev()
{
    if (m_DirtyUIDList.Num() > 0)
    {
        ShowDirtyPopup(FOnBasePopupMainEvent::CreateUObject(this, &UCorpsMain::CallBackDirtyPrev));
        return;
    }
    UWLGameUISystem* GameUISystem = UWLFunctionLibrary_System::GetGameUISystem(this);
    CHECK_INVALID_QUIT(GameUISystem);
    HidePopup();
    GameUISystem->PrevGnb();
}

void UCorpsMain::CallBackDirtyPrev()
{
    m_DirtyUIDList.Empty();
    CallBackPrev();
}

void UCorpsMain::CallBackExit()
{
    if (m_DirtyUIDList.Num() > 0)
    {
        ShowDirtyPopup(FOnBasePopupMainEvent::CreateUObject(this, &UCorpsMain::CallBackDirtyExit));
        return;
    }
    UWLGameUISystem* GameUISystem = UWLFunctionLibrary_System::GetGameUISystem(this);
    CHECK_INVALID_QUIT(GameUISystem);
    HidePopup();
    GameUISystem->Show(EUINameType::frm_Battleview);
    GameUISystem->Hide(EUINameType::frm_Gnb);
    GameUISystem->Hide(this->WidgetNameType);
}
void UCorpsMain::CallBackDirtyExit()
{
    m_DirtyUIDList.Empty();
    CallBackExit();
}