// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryMain.h"

#include "Components/WidgetSwitcher.h"
#include "Components/TileView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "FrameWork/MainGameInstance.h"

#include "Define/EnumDefine.h"

#include "System/Reddot/ReddotSystem.h"

#include "FunctionLibrary/CharacterFunctionLibrary.h"
#include "FunctionLibrary/ItemFunctionLibrary.h"

#include "Data/Item/ItemData.h"
#include "Data/Item/ItemDataInventory.h"

#include "UI/ListItem/Tab_ListItem.h"
#include "UI/ListItem/ItemData_ListItem.h"

#pragma region Base
void UInventoryMain::Init()
{
    if (Bt_Auto == nullptr || Bt_Auto->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Bt_Auto is nullptr"));
        return;
    }
    if (Bt_Dump == nullptr || Bt_Dump->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Bt_Dump is nullptr"));
        return;
    }
    if (Bt_Close_Dump == nullptr || Bt_Close_Dump->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Bt_Close_Dump is nullptr"));
        return;
    }
    if (Bt_Filter_Dump == nullptr || Bt_Filter_Dump->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Bt_Filter_Dump is nullptr"));
        return;
    }
    if (TileView_Item == nullptr || TileView_Item->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Item is nullptr"));
        return;
    }
    if (TileView_Dump == nullptr || TileView_Dump->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Dump is nullptr"));
        return;
    }

    //=========================================================================================================
    UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MainGameInstance == nullptr || MainGameInstance->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainGameInstance is nullptr"));
        return;
    }

    //=========================================================================================================
    UItemDataInventory* ItemDataInventory = MainGameInstance->GetItemDataInventory();
    if (ItemDataInventory == nullptr || ItemDataInventory->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataInventory is nullptr"));
        return;
    }

    //=========================================================================================================
    //	event binding.
    Bt_Auto->OnClicked.AddUniqueDynamic(this, &UInventoryMain::OnClicked_Bt_Auto);
    Bt_Dump->OnClicked.AddUniqueDynamic(this, &UInventoryMain::OnClicked_Bt_Dump);
    Bt_Close_Dump->OnClicked.AddUniqueDynamic(this, &UInventoryMain::OnClicked_Bt_Close_Dump);
    Bt_Filter_Dump->OnClicked.AddUniqueDynamic(this, &UInventoryMain::OnClicked_Bt_Filter_Dump);
    TileView_Item->OnItemClicked().AddUObject(this, &UInventoryMain::Callback_ItemSelect);
    TileView_Dump->OnItemClicked().AddUObject(this, &UInventoryMain::Callback_ItemSelect);

    ItemDataInventory->InventoryUpdate.AddUObject(this, &UInventoryMain::UpdateInventory);

    //=========================================================================================================
    //  ó�� ������ ���� ���â�� ����.
    BuildDataByUser(UInventoryMain::EInventoryViewType::Equip);
}

void UInventoryMain::Shutdown()
{
    if (Bt_Auto != nullptr && Bt_Auto->IsValidLowLevel())
    {
        Bt_Auto->OnClicked.Clear();
    }
    if (Bt_Dump != nullptr && Bt_Dump->IsValidLowLevel())
    {
        Bt_Dump->OnClicked.Clear();
    }
    if (Bt_Close_Dump != nullptr && Bt_Close_Dump->IsValidLowLevel())
    {
        Bt_Close_Dump->OnClicked.Clear();
    }
    if (Bt_Filter_Dump != nullptr && Bt_Filter_Dump->IsValidLowLevel())
    {
        Bt_Filter_Dump->OnClicked.Clear();
    }
    if (TileView_Item != nullptr && TileView_Item->IsValidLowLevel())
    {
        TileView_Item->OnClicked.Clear();
    }
    if (TileView_Dump != nullptr && TileView_Dump->IsValidLowLevel())
    {
        TileView_Dump->OnClicked.Clear();
    }

    //=========================================================================================================
    UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MainGameInstance != nullptr && MainGameInstance->IsValidLowLevel())
    {
        UItemDataInventory* ItemDataInventory = MainGameInstance->GetItemDataInventory();
        if (ItemDataInventory != nullptr && ItemDataInventory->IsValidLowLevel())
        {
            ItemDataInventory->InventoryUpdate.RemoveAll(this);
        }
    }
}

#pragma endregion

// public.
//===============================================================================================================================================================================
// private.

#pragma region Start
void UInventoryMain::BuildDataByUser(const UInventoryMain::EInventoryViewType& InViewType)
{
    if (InViewType == m_ViewType)
    {
        // �̹� ������.
        return;
    }

    //=========================================================================================================
    if (WidgetSwitcher_Popup == nullptr || WidgetSwitcher_Popup->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("WidgetSwitcher_Popup is nullptr"));
        return;
    }

    //=========================================================================================================
    m_ViewType = InViewType;

    //=========================================================================================================
    switch (m_ViewType)
    {
    case EInventoryViewType::View_Equip:
    {
        WidgetSwitcher_Popup->SetActiveWidgetIndex(0);

        UpdateInventory();
    }
    break;
    case EInventoryViewType::View_Dump:
    {
        WidgetSwitcher_Popup->SetActiveWidgetIndex(1);

        UpdateDump();
    }
    break;
    default:
    {
    }
    break;
    }
}

#pragma endregion

#pragma region Tab
void UInventoryMain::SetTab()
{
    if (0 < m_TabList.Num())
    {
        //  �ٽ� �� �ʿ� ����.
        return;
    }

    const UEnum* TabTypeEnum = FindObject(ANY_PACKAGE, TEXT("ETabType"), true);
    if (TabTypeEnum == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("TabTypeEnum is nullptr"));
        return;
    }

    for (int32 i = 0; i < TabTypeEnum->NumEnums(); i++)
    {
        UTab_ListItem* TabWidget = Cast<UTab_ListItem>(GetWidgetFromName(FName(*FString::Printf(TEXT("ListItem_Tab_%d"), i))));
        if (nullptr == TabWidget || false == TabWidget->IsValidLowLevel())
        {
            UE_LOG(LogTemp, Warning, TEXT("TabWidget is nullptr"));
            continue;
        }

        TabWidget->Init();
        TabWidget->SetOnTabClickEvent(UTab_ListItem::FOnClickTab::CreateUObject(this, &UInventoryMain::ChangeTab));
        TabWidget->SetTabType(TabTypeEnum->GetValueByIndex(i));

        m_TabList.Emplace(TabTypeEnum->GetValueByIndex(i), TabWidget);
    }
}

void UInventoryMain::ChangeTab(ETabType InTabType)
{
    if (m_ActiveTabType == InTabType)
    {
        //  ������.
        return;
    }

    //=========================================================================================================
    // set ui.
    SetTabUI(m_ActiveTabType, InTabType);
    // set data.
    m_ActiveTabType = InTabType;
    // update item widget.
    UpdateAllItem();
    // update count.
    SetCountText();
}

void UWLInventoryPopup_Form::SetTabUI(const ETabType& InOffTabType, const ETabType& InOnTabType)
{
    if (m_TabList.Contains(InOffTabType) && m_TabList[InOffTabType] != nullptr && m_TabList[InOffTabType]->IsValidLowLevel())
    {
        m_TabList[InOffTabType]->SetActive(false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("m_TabList[InOffTabType] is nullptr"));
        return;
    }

    //=========================================================================================================
    if (m_TabList.Contains(InOnTabType) && m_TabList[InOnTabType] != nullptr && m_TabList[InOnTabType]->IsValidLowLevel())
    {
        m_TabList[InOnTabType]->SetActive(false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("m_TabList[InOnTabType] is nullptr"));
        return;
    }
}

#pragma endregion

#pragma region Count Text
void UInventoryMain::SetCountText()
{
    if (Text_Value == nullptr || Text_Value->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Text_Value is nullptr"));
        return;
    }

    //=========================================================================================================
    UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MainGameInstance == nullptr || MainGameInstance->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainGameInstance is nullptr"));
        return;
    }

    //=========================================================================================================
    UItemDataInventory* ItemDataInventory = MainGameInstance->GetItemDataInventory();
    if (ItemDataInventory == nullptr || ItemDataInventory->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataInventory is nullptr"));
        return;
    }

    //=========================================================================================================
    //  set text.
    Text_Value->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), ItemDataList.Num(), ItemDataInventory->GetInventoryCount())));
}

void UInventoryMain::SetDumpCountText()
{
    if (Text_Value_Dump == nullptr || Text_Value_Dump->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Text_Value_Dump is nullptr"));
        return;
    }
    
    //=========================================================================================================
    //  set text.
    Text_Value_Dump->SetText(FText::FromString(FString::Printf(TEXT("(%d/%d)"), m_DumpSelectList.Num(), ItemDataList.Num())));
}
#pragma endregion

#pragma region Filter
bool UInventoryMain::FilterItem(UItemData* InItem)
{
    if (InItem == nullptr || InItem->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("InItem is nullptr"));
        return false;
    }

    //=========================================================================================================
    switch (m_ActiveTabType)
    {
    case ETabType::All:
    {
        return true;
    }
    break;
    case ETabType::Weapon:
    {
        return (bool)(UItemFunctionLibrary::GetItemType(InItem->GetDataID()) == EItemType::Weapon);
    }
    break;
    case ETabType::Armor:
    {
        if ((bool)(UItemFunctionLibrary::GetItemType(InItem->GetDataID()) == EItemType::Helmet))
        {
            return true;
        }
        else if ((bool)(UItemFunctionLibrary::GetItemType(InItem->GetDataID()) == EItemType::Armor))
        {
            return true;
        }
    }
    break;
    case ETabType::Accessories:
    {
        if ((bool)(UItemFunctionLibrary::GetItemType(InItem->GetDataID()) == EItemType::Ring))
        {
            return true;
        }
        else if ((bool)(UItemFunctionLibrary::GetItemType(InItem->GetDataID()) == EItemType::Necklace))
        {
            return true;
        }
    }
    break;
    case ETabType::Consumables:
    {
        return (bool)(UItemFunctionLibrary::IsEquip(InItem->GetDataID()) == false);
    }
    break;
    default:
        break;
    }

    return false;
}

#pragma endregion

#pragma region Item
void UInventoryMain::ClearListItems()
{
    if (TileView_Item == nullptr || TileView_Item->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Item is nullptr"));
        return;
    }
    if (TileView_Dump == nullptr || TileView_Dump->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Dump is nullptr"));
        return;
    }

    //=========================================================================================================
    for (UObject* Widget : TileView_Item->GetListItems())
    {
        UListItem_Item_Data* ItemData = Cast<UListItem_Item_Data>(Widget);
        if (nullptr == ItemData || false == ItemData->IsValidLowLevel())
        {
            continue;
        }

        PoolDataList.AddUnique(ItemData);
    }

    for (UObject* Widget : TileView_Dump->GetListItems())
    {
        UListItem_Item_Data* ItemData = Cast<UListItem_Item_Data>(Widget);
        if (nullptr == ItemData || false == ItemData->IsValidLowLevel())
        {
            continue;
        }

        PoolDataList.AddUnique(ItemData);
    }

    //=========================================================================================================
    // remove.
    ItemDataList.Empty();
    TileView_Item->ClearListItems();
    TileView_Dump->ClearListItems();
}

void UInventoryMain::UpdateInventory()
{
    // update item.
    UpdateAllItem();

    // update count.
    SetCountText();
}

void UInventoryMain::UpdateDump()
{
    // update data.
    m_DumpSelectList.Empty();

    // update item.
    UpdateAllItem();

    // update count.
    SetDumpCountText();
}

void UInventoryMain::UpdateItem(const int64& InItemUID)
{
    if (InItemUID <= 0)
    {
        WL_LOG(Error, TEXT("InItemUID is 0"));
        return;
    }

    if (m_ViewType != EInventoryViewType::View_Equip)
    {
        //  �������� ������ ���� ���� ���� �������� ����� ���ŵ��� ����.
        WL_LOG(Error, TEXT("EInventoryViewType::View_Equip is false"));
        return;
    }

    //=========================================================================================================
    if (TileView_Item == nullptr || TileView_Item->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Item is nullptr"));
        return;
    }

    //=========================================================================================================
    UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MainGameInstance == nullptr || MainGameInstance->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainGameInstance is nullptr"));
        return;
    }

    //=========================================================================================================
    UItemDataInventory* ItemDataInventory = MainGameInstance->GetItemDataInventory();
    if (ItemDataInventory == nullptr || ItemDataInventory->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataInventory is nullptr"));
        return;
    }

    //=========================================================================================================
    UReddotSystem* ReddotSystem = MainGameInstance->GetReddotSystem();
    if (ReddotSystem == nullptr || ReddotSystem->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ReddotSystem is nullptr"));
        return;
    }

    //=========================================================================================================
    UItemData* UpdateItemData = ItemDataInventory->GetItemByUID(InItemUID);

    // ������ ���� �������� ��.
    if (ItemDataList.Contains(InItemUID) == false)
    {
        // �� �� ������ return;
        if (UpdateItemData == nullptr || UpdateItemData->IsValidLowLevel() == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("UpdateItemData is nullptr"));
            return;
        }

        // ���� ����� �ʾƵ� �Ǹ� return;
        if (FilterItem(UpdateItemData) == false)
        {
            return;
        }

        // add.
        AddItem(ReddotSystem, UpdateItemData);

        // sync.
        SyncEmptyItem();

        // count.
        SetCountText();

        // view widget update.
        UItemData_ListItem* ViewItem = Cast<UItemData_ListItem>(TileView_Item->GetEntryWidgetFromItem(ItemDataList[InItemUID]));
        if (ViewItem != nullptr && ViewItem->IsValidLowLevel() == true)
        {
            // view item�� ���� �� ����. ���� �ƴ�.
            ViewItem->UpdateUI();
        }
    }
    else
    {
        // �ִµ� ���Ϳ� �ɸ��� ������.
        if (FilterItem(UpdateItemData))
        {
            // ������ ������Ʈ.
            // set data.
            ItemDataList[InItemUID]->ItemData = UpdateItemData;
            ItemDataList[InItemUID]->IsEquip = (bool)(UpdateItemData->GetEquipTargetUID() > 0);
            ItemDataList[InItemUID]->IsSelect = false;

            // view widget update.
            UItemData_ListItem* ViewItem = Cast<UItemData_ListItem>(TileView_Item->GetEntryWidgetFromItem(ItemDataList[InItemUID]));
            if (ViewItem != nullptr && ViewItem->IsValidLowLevel() == true)
            {
                // view item�� ���� �� ����. ���� �ƴ�.
                ViewItem->UpdateUI();
            }
        }
        else
        {
            // ������ �����ϱ�.
            // save.
            PoolDataList.AddUnique(ItemDataList[InItemUID]);

            // remove.
            TileView_Item->RemoveItem(ItemDataList[InItemUID]);
            ItemDataList.Remove(InItemUID);

            // sync.
            SyncEmptyItem();

            // count.
            SetCountText();
        }
    }
}

void UInventoryMain::UpdateAllItem()
{
    if (TileView_Item == nullptr || TileView_Item->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Item is nullptr"));
        return;
    }

    if (TileView_Dump == nullptr || TileView_Dump->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Dump is nullptr"));
        return;
    }

    //=========================================================================================================
    UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MainGameInstance == nullptr || MainGameInstance->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainGameInstance is nullptr"));
        return;
    }

    //=========================================================================================================
    UItemDataInventory* ItemDataInventory = MainGameInstance->GetItemDataInventory();
    if (ItemDataInventory == nullptr || ItemDataInventory->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataInventory is nullptr"));
        return;
    }

    //=========================================================================================================
    UReddotSystem* ReddotSystem = MainGameInstance->GetReddotSystem();
    if (ReddotSystem == nullptr || ReddotSystem->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ReddotSystem is nullptr"));
        return;
    }

    //=========================================================================================================
    //  clear.
    ClearListItems();

    //=========================================================================================================
    //  get data.
    TArray<UItemData*> InventoryItemList;
    ItemDataInventory->GetAllItems(InventoryItemList);

    //  sort.
    InventoryItemList.Sort([](UItemData& A, UItemData& B) { return (bool)(A.GetUID() < B.GetUID()); });

    // add item.
    for (UItemData* ItemData : InventoryItemList)
    {
        if (ItemData == nullptr || ItemData->IsValidLowLevel() == false)
        {
            continue;
        }

        if (FilterItem(ItemData) == false)
        {
            continue;
        }
      
        AddItem(ReddotSystem, ItemData);
    }

    //=========================================================================================================
    // add empty widget.
    SyncEmptyItem();

    //=========================================================================================================
    //  update tileview.
    if (m_ViewType == EInventoryViewType::Equip)
    {
        // regenerate.
        TileView_Item->RegenerateAllEntries();
    
        // scroll.
        TileView_Item->ScrollToTop();
    }
    else
    {
        // regenerate.
        TileView_Dump->RegenerateAllEntries();
    
        // scroll.
        TileView_Dump->ScrollToTop();
    }
}

void UInventoryMain::AddItem(UReddotSystem* InReddotSystem, UItemData* InItemData)
{
    if (InReddotSystem == nullptr || InReddotSystem->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("InReddotSystem is nullptr"));
        return;
    }
    if (InItemData == nullptr || InItemData->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("InItemData is nullptr"));
        return;
    }
    if (TileView_Item == nullptr || TileView_Item->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Item is nullptr"));
        return;
    }
    if (TileView_Dump == nullptr || TileView_Dump->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Dump is nullptr"));
        return;
    }

    //=========================================================================================================
    UListItem_Item_Data* NewData = nullptr;
    if (ItemDataList.Contains(InItemData->GetUID()))
    {
        //  ������ �����Ͱ� ������ �״�� ������ ���.
        NewData = ItemDataList[InItemData->GetUID()];
    }
    else if (0 < PoolDataList.Num())
    {
        //  ������ PoolDataList���� ������ ���.
        for (UListItem_Item_Data* ItemWidget : PoolDataList)
        {
            if (INDEX_NONE != TileView_Item->GetIndexForItem(ItemWidget))
            {
                continue;
            }
            if (INDEX_NONE != TileView_Dump->GetIndexForItem(ItemWidget))
            {
                continue;
            }

            NewData = ItemWidget;
            PoolDataList.Remove(NewData);
            break;
        }
    }
    else
    {
        //  PoolDataList�� ����ִ� ���¶�� ���� ����.
        NewData = NewObject<UListItem_Item_Data>(this);
    }

    if (NewData == nullptr || NewData->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("NewData is nullptr"));
        return;
    }

    //=========================================================================================================
    // set data.
    NewData->ItemData = InItemData;
    NewData->IsSelect = false;
    NewData->IsReddot = InReddotSystem->GetReddot(ERedDotConditions::Inventory, InItemData->GetUID();

    //=========================================================================================================
    // add.
    if (m_ViewType == EInventoryViewType::Equip)
    {
        TileView_Item->AddItem(NewData);
    }
    else
    {
        TileView_Dump->AddItem(NewData);
    }

    ItemDataList.Emplace(InItemData->GetUID(), NewData);
}

void UInventoryMain::SyncEmptyItem()
{
    if (m_ViewType == EInventoryViewType::Dump)
    {
        //  ������� �� �ʿ� ����.
        UE_LOG(LogTemp, Warning, TEXT("ViewType is Dump"));
        return;
    }

    if (TileView_Item == nullptr || TileView_Item->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileView_Item is nullptr"));
        return;
    }

    //=========================================================================================================
    UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MainGameInstance == nullptr || MainGameInstance->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainGameInstance is nullptr"));
        return;
    }

    //=========================================================================================================
    UItemDataInventory* ItemDataInventory = MainGameInstance->GetItemDataInventory();
    if (ItemDataInventory == nullptr || ItemDataInventory->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataInventory is nullptr"));
        return;
    }

    //=========================================================================================================
    // additem���� widget�� �߰����� �� ���� �߰��� empty�� ���� ������ �и� ��츦 ���� ����.
    TArray<UListItem_Item_Data*> EmptyList;

    // i�� if�� �ȿ��� ������.
    for (int32 i = 0; i < TileView_Item->GetListItems().Num();)
    {
        UListItem_Item_Data* ItemData = Cast<UListItem_Item_Data>(TileView_Item->GetItemAt(i));
        if (nullptr == ItemData || false == ItemData->IsValidLowLevel())
        {
            break;
        }

        if (ItemData->ItemData != nullptr)
        {
            i++;
            continue;
        }

        // save.
        EmptyList.AddUnique(ItemData);
        // remove.
        TileView_Item->RemoveItem(ItemData);
    }
    
    // add empty.
    for (UListItem_Item_Data* ItemData : EmptyList)
    {
        TileView_Item->AddItem(ItemData);
    }

    //=========================================================================================================
    // match empty widget.
    if (ItemDataInventory->GetInventoryCount() > TileView_Item->GetListItems().Num())
    {
        // �߰���.
        while (ItemDataInventory->GetInventoryCount() > TileView_Item->GetListItems().Num())
        {
            UListItem_Item_Data* EmptyData = nullptr;
            if (0 < PoolDataList.Num())
            {
                for (UListItem_Item_Data* ItemWidget : PoolDataList)
                {
                    if (INDEX_NONE != TileView_Item->GetIndexForItem(ItemWidget))
                    {
                        continue;
                    }
                    
                    EmptyData = ItemWidget;
                
                    break;
                }

                PoolDataList.Remove(EmptyData);
            }
            else
            {
                EmptyData = NewObject<UListItem_Item_Data>(this);
            }

            EmptyData->SetItemData(nullptr);
            EmptyData->SetSelect(false);

            //=========================================================================================================
            // add.
            TileView_Item->AddItem(EmptyData);
        }
    }
    else
    {
        // �տ������� ������.
        int32 Index = 0;
        while (ItemDataInventory->GetInventoryCount() < TileView_Item->GetListItems().Num())
        {
            UListItem_Item_Data* ItemData = Cast<UListItem_Item_Data>(TileView_Item->GetItemAt(Index));
            if (nullptr == ItemData || false == ItemData->IsValidLowLevel())
            {
                break;
            }
            if (nullptr != ItemData->ItemData)
            {
                Index++;
                continue;
            }

            // save.
            PoolDataList.AddUnique(ItemData);
            // remove.
            TileView_Item->RemoveItem(ItemData);
        }
    }
}

void UInventoryMain::SelectItem(const int64& InItemUID, const bool InbSelect)
{
    if (InItemUID <= 0)
    {
        return;
    }

    if (ItemDataList.Contains(InItemUID))
    {
        ItemDataList[InItemUID]->IsSelect = InbSelect;

        // view widget update.
        UItemData_ListItem* ViewItem = nullptr;
        if (m_ViewType == EInventoryViewType::Equip)
        {
            ViewItem = Cast<UItemData_ListItem>(TileView_Item->GetEntryWidgetFromItem(ItemDataList[InItemUID]));
        }
        else
        {
            ViewItem = Cast<UItemData_ListItem>(TileView_Dump->GetEntryWidgetFromItem(ItemDataList[InItemUID]));
        }

        if (nullptr != ViewItem && ViewItem->IsValidLowLevel())
        {
            // view item�� ���� �� ����. ���� �ƴ�.
            ViewItem->UpdateUI();
        }
    }
}

#pragma endregion

#pragma region ListView
void UInventoryMain::Callback_ItemSelect(UObject* InItemObject)
{
    if (InItemObject == nullptr || InItemObject->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("InItem is nullptr"));
        return;
    }

    UItemData_ListItem* ItemBox = Cast<UItemData_ListItem>(InItemObject);
    if (ItemBox == nullptr || ItemBox->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemBox is nullptr"));
        return;
    }

    if (InItem->GetItemData() == nullptr || InItem->GetItemData()->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("InItem->GetItemData() is nullptr"));
        return;
    }

    //=========================================================================================================
    if (m_ViewType == EInventoryViewType::Equip)
    {
        Callback_ItemSelect_ViewEquip(InItem->GetItemData()->GetUID());
    }
    else
    {
        Callback_ItemSelect_ViewDump(InItem->GetItemData()->GetUID());
    }
}

void UInventoryMain::Callback_ItemSelect_ViewEquip(const int64& InItemUID)
{
    //  deselect.
    if (0 < m_SelectItemUID)
    {
        SelectItem(m_SelectItemUID, false);
    }
    
    //=========================================================================================================
    //  select.
    m_SelectItemUID = InItemUID;

    SelectItem(m_SelectItemUID, true);

    //=========================================================================================================
    // delegate.
    // ��â Ȱ��ȭ.
    m_OnItemClick.ExecuteIfBound(m_SelectItemUID);
}

void UInventoryMain::Callback_ItemSelect_ViewDump(const int64& InItemUID)
{
    //  update.
    if (m_DumpSelectList.Contains(InItemUID) == false)
    {
        //  ���� ���õ��� ���� �������̶�� ���� ����.
        m_DumpSelectList.Emplace(InItemUID);
        SelectItem(InItemUID, true);
    }
    else
    {
        //  ������ �Ǿ� �ִ� ���¶�� ���� ���� ������.
        m_DumpSelectList.Remove(InItemUID);
        SelectItem(InItemUID, false);
    }

    //=========================================================================================================
    //  update count.
    SetDumpCountText();
}

#pragma endregion

#pragma region AutoEquip
void UInventoryMain::OnClickAutoEquip()
{
    UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MainGameInstance == nullptr || MainGameInstance->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainGameInstance is nullptr"));
        return;
    }

    //=========================================================================================================
    UItemDataInventory* ItemDataInventory = MainGameInstance->GetItemDataInventory();
    if (ItemDataInventory == nullptr || ItemDataInventory->IsValidLowLevel() == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataInventory is nullptr"));
        return;
    }

    //=========================================================================================================
    TArray<UItemData*> ChangeItemList;
    TArray<EItemType> EquipTypeList;
    EquipTypeList.Emplace(EItemType::Weapon);
    EquipTypeList.Emplace(EItemType::Helmet);
    EquipTypeList.Emplace(EItemType::Armor);
    EquipTypeList.Emplace(EItemType::Ring);
    EquipTypeList.Emplace(EItemType::Necklace);

    for (const EItemType& ItemType : EquipTypeList)
    {
        TArray<UItemData*> ItemList;
        if (ItemDataInventory->GetItemListByItemType(ItemType, ItemList) == false)
        {
            //  �ش� Ÿ���� ��� ����.
            continue;
        }

        ItemList.RemoveAll([&](UItemData* A)
            {
                //  already equip remove.
                if (UCharacterFunctionLibrary::IsEquip(A->GetUID()))
                {
                    return true;
                }

                return (bool)(UCharacterFunctionLibrary::IsStrongerThanEquipFitted(A->GetUID()) == false);
            });

        if (ItemList.Num() > 0)
        {
            ItemList.Sort([](UItemData& A, UItemData& B)
                {
                    return (bool)(UItemFunctionLibrary::GetCombatPower(A.GetUID()) > UItemFunctionLibrary::GetCombatPower(B.GetUID()));
                });

            ChangeItemList.Emplace(ItemList[0]);
        }
    }

    //=========================================================================================================
    //  TODO : network send.
}

#pragma endregion

// private.
//===============================================================================================================================================================================
// private - button funtcion.

#pragma region Button_Event
void UInventoryMain::OnClicked_Bt_Auto()
{
    OnClickAutoEquip();
}

void UInventoryMain::OnClicked_Bt_Dump()
{
    BuildDataByUser(UInventoryMain::EInventoryViewType::View_Dump);
}

void UInventoryMain::OnClicked_Bt_Close_Dump()
{
    m_DumpSelectList.Empty();

    BuildDataByUser(UInventoryMain::EInventoryViewType::View_Equip);
}

void UInventoryMain::OnClicked_Bt_Filter_Dump()
{
    m_OnDumpFilter.ExecuteIfBound();
}

#pragma endregion