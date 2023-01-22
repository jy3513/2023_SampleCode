// Fill out your copyright notice in the Description page of Project Settings.

#include "Mail_Main.h"

#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"

#define MAX_LOAD_COUNT 100
#define LIMIT_CONTENT 24

void UMail_Main::Init()
{
#pragma region BIND_WIDGET
	if (Bt_AllReceive == nullptr || Bt_AllReceive->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bt_AllReceive is valid false"));
		return;
	}
	if (Bt_Delete == nullptr || Bt_Delete->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bt_Delete is valid false"));
		return;
	}
	
	Bt_AllReceive->SetActive(true);
	Bt_Delete->SetActive(true);

#pragma endregion

	EventConnect();

	SetUp();
}

void UMail_Main::EventConnect()
{
	if (Bt_Close == nullptr || Bt_Close->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bt_Close is valid false"));
		return;
	}
	if (Bt_AllReceive == nullptr || Bt_AllReceive->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bt_AllReceive is valid false"));
		return;
	}
	if (Bt_Delete == nullptr || Bt_Delete->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bt_Delete is valid false"));
		return;
	}

	Bt_Close->OnClicked.AddUniqueDynamic(this, &UMail_Main::OnClicked_Bt_Close);
	Bt_AllReceive->OnClicked.AddUniqueDynamic(this, &UMail_Main::OnClicked_Bt_AllReceive);
	Bt_Delete->OnClicked.AddUniqueDynamic(this, &UMail_Main::OnClicked_Bt_Delete);
}

void UMail_Main::ReleaseWidget()
{
	if (Bt_Close != nullptr)
	{
		Bt_Close->OnClicked.Clear();
	}
	if (Bt_AllReceive != nullptr)
	{
		Bt_AllReceive->OnClicked.Clear();
	}
	if (Bt_Delete != nullptr)
	{
		Bt_Delete->OnClicked.Clear();
	}
}

void UMail_Main::CallbackMailList(const int32& InMailTotalCount, const TArray<UMail_Item_Data*>& InMailList)
{
	m_TotalCount = InMailTotalCount;
	m_MailItemList.Append(InMailList);

	UpdateMail();
}

void UMail_Main::CallbackMailReceive(const int64& InMailUID, const bool IsGetReceive)
{
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return;
	}

	//=========================================================================================================
	// update data.
	for (const TPair<int64, UMail_Item_Data*>& MailInfo : m_MailItemList)
	{
		if (InMailUID != MailInfo.Value->m_MailUID)
		{
			continue;
		}

		m_ReceivableCount--;
		MailInfo.Value->m_IsComplete = true;

		if (m_MailItemList.Contains(MailInfo.Value->m_MailUID) == true)
		{
			if (m_MailItemList[MailInfo.Value->m_MailUID] == nullptr || m_MailItemList[MailInfo.Value->m_MailUID]->IsValidLowLevel() == false)
			{
				continue;
			}
			
			m_MailItemList[MailInfo.Value->m_MailUID]->m_IsComplete = true;

			// view widget update.
			UMail_Item* ViewItem = Cast<UMail_Item>(ListView_MailList->GetEntryWidgetFromItem(m_MailItemList[MailInfo.Value->m_MailUID]));
			if (ViewItem != nullptr && ViewItem->IsValidLowLevel() == true)
			{
				// view item이 없을 수 있음. 에러 아님.
				ViewItem->UpdateMail();
			}
		}

		break;
	}

	UpdateMailUI();
}

void UMail_Main::CallbackMailDelete(const int64& InMailUID)
{
	m_TotalCount--;

	RemoveMail(InMailUID);

	UpdateMailUI();
}

// public.
//===============================================================================================================================================================================
// private.
void UMail_Main::SetUp()
{
	//	reset.
	ClearListItems();
	//	network send.
	SendMailList();
}

#pragma region ItemWidget
bool UMail_Main::RemoveMail(const int64& InMailUID)
{
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return false;
	}

	//=========================================================================================================
	if (m_MailItemList.Contains(InMailUID) == false)
	{
		return false;
	}

	// view widget remove.
	ListView_MailList->RemoveItem(m_MailItemList[InMailUID]);
	m_MailItemList.Remove(InMailUID);

	return true;
}

void UMail_Main::ClearListItems()
{
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return;
	}

	//=========================================================================================================
	// remove.
	m_MailItemList.Empty();
	ListView_MailList->ClearListItems();
}

void UMail_Main::AddMail(const UMail_Item_Data* InMailInfo)
{
	if (MailData == nullptr || MailData->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("MailData is valid false"));
		return;
	}

	if (InMailInfo->m_MailUID <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("MailData uid is 0"));
		return;
	}
	
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return;
	}

	if (ListView_MailList->GetNumItems() >= MAX_LOAD_COUNT)
	{
		// 더이상 넣을 수 없음.
		return;
	}

	//=========================================================================================================
	// set event.
	if (MailWidget->IsOnClickCheckValid() == false)
	{
		MailWidget->SetOnClickEvent(UMail_Item::FOnClickEvent::CreateUObject(this, &UMail_Main::CallbackMail));
	}

	//=========================================================================================================
	// add.
	m_MailItemList.Emplace(MailData->m_MailUID, MailData);
	ListView_MailList->AddItem(MailData);
}

void UMail_Main::CallbackMail(const int64& InMailUID)
{
	if (m_MailItemList.Contains(InMailUID) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("m_MailItemList Contains is false [ index : %d ] "), InMailUID);
		return;
	}

	if (m_MailItemList[InMailUID]->m_IsComplete == false)
	{
		SendMailReceive(InMailUID);
	}
	else
	{
		SendMailDelete(InMailUID);
	}
}

#pragma endregion

#pragma region UpdateItem
void UMail_Main::UpdateMail()
{
	// item widget.
	UpdateAllItem();
	// update ui.
	UpdateMailUI();
}

void UMail_Main::UpdateMailUI()
{
	// show empty text.
	CheckEmptyUI();
	// set mail count.
	SetMailCount();
}

void UMail_Main::UpdateAllItem()
{
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return;
	}

	ClearListItems();
	m_ReceivableCount = 0;
	for (const TPair<int64, UMail_Item_Data*>& MailInfo : m_MailItemList)
	{
		if (MailInfo.Value->m_IsComplete == false)
		{
			m_ReceivableCount++;
		}

		AddMail(MailInfo.Value);
	}

	ListView_MailList->RegenerateAllEntries();
}

#pragma endregion

#pragma region Mail_UMG
void UMail_Main::CheckEmptyUI()
{
	if (CanvasPanel_Empty == nullptr || CanvasPanel_Empty->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanvasPanel_Empty is valid false"));
		return;
	}
	if (Bt_AllReceive == nullptr || Bt_AllReceive->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bt_AllReceive is valid false"));
		return;
	}
	if (Bt_Delete == nullptr || Bt_Delete->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bt_Delete is valid false"));
		return;
	}

	//=========================================================================================================
	// set empty.
	CanvasPanel_Empty->SetVisibility((bool)(m_TotalCount <= 0) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	Bt_AllReceive->SetActive((bool)(m_ReceivableCount > 0));
	Bt_Delete->SetActive((bool)(m_TotalCount > 0));
}

void UMail_Main::SetMailCount()
{
	if (Text_Receive_Num == nullptr || Text_Receive_Num->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Text_Receive_Num is valid false"));
		return;
	}

	Text_Receive_Num->SetText(FText::AsNumber(m_TotalCount > MAX_LOAD_COUNT ? MAX_LOAD_COUNT : m_TotalCount));
}

#pragma endregion

#pragma region Network_Send_Function
void UMail_Main::SendMailList()
{
	//  TODO : network send.
}

void UMail_Main::SendMailReceive(const int64& InMailUID)
{
	//  TODO : network send.
}

void UMail_Main::SendMailDelete(const int64& InMailUID)
{
	//  TODO : network send.
}

void UMail_Main::SendMailAllReceive()
{
	//  TODO : network send.
}

void UMail_Main::SendMailAllDelete()
{
	//  TODO : network send.
}

#pragma endregion

#pragma region Button_Event
void UMail_Main::OnClicked_Bt_Close()
{
	this->RemoveToParent();
}

void UMail_Main::OnClicked_Bt_AllReceive()
{
	if (m_MailItemList.Num() <= 0 || m_ReceivableCount <= 0)
	{
		return;
	}

	SendMailAllReceive();
}

void UMail_Main::OnClicked_Bt_Delete()
{
	if (m_MailItemList.Num() <= 0 || m_TotalCount <= 0)
	{
		return;
	}

	SendMailAllDelete();
}

#pragma endregion

#undef MAX_LOAD_COUNT
#undef LIMIT_CONTENT
