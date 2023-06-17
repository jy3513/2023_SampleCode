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
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return;
	}

	//=========================================================================================================
	//	event binding.
	Bt_Close->OnClicked.AddUniqueDynamic(this, &UMail_Main::OnClicked_Bt_Close);
	Bt_AllReceive->OnClicked.AddUniqueDynamic(this, &UMail_Main::OnClicked_Bt_AllReceive);
	Bt_Delete->OnClicked.AddUniqueDynamic(this, &UMail_Main::OnClicked_Bt_Delete);
	ListView_MailList->OnItemClicked().AddUObject(this, &UMail_Main::Callback_MailButton);
	
	//=========================================================================================================
	//	network send.
	SendMail_List();
}

#pragma region Callback_Function
void UMail_Main::Callback_MailList(const TArray<UMail_IteData*>& InMailList)
{
	//	�ӽ� ������ ����.
	MailItemList.Empty();
	MailItemList.Append(InMailList);

	//	�����Ϳ� UI ��� ����.
	UpdateMail();
}

void UMail_Main::Callback_MailReceive(const int64& InMailUID)
{
	//	������ �޾����� ReceivableCount���� ����.
	ReceivableCount--;

	//	�����͸� ���� ���� ������ ���� �� �� �ش� ������ UI�� ���� ������.
	ReceiveMail(InMailUID);
	
	//	ReceivableCount�� ����Ǿ����� UI�� ���� ������.
	UpdateMailCountUI();
}

void UMail_Main::Callback_MailDelete(const int64& InMailUID)
{
	//	������ ���� �Ǿ����� TotalCount���� ����.
	TotalCount--;

	//	������ ���� ó���� �ش� ������ UI�� ���� ó����.
	RemoveMail(InMailUID);

	//	TotalCount�� ���� �Ǿ����� UI�� ���� ������.
	UpdateMailCountUI();
}

void UMail_Main::Callback_MailButton(UObject* InMailObject)
{
	if (InMailObject == nullptr || InMailObject->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("InMailObject is valid false"));
		return;
	}

	UMail_Item_Data* MailData = Cast<UMail_Item_Data>(InMailObject);
	if (MailData == nullptr || MailData->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("MailData is valid false"));
		return;
	}

	/**
	* ������ ������ �޾Ҵ��� Ȯ��.
	* 1. ������ ���� �ʾ����� ���� ���� �޴� Req�� ������ �Լ� ȣ��.
	* 2. ������ ���� �Ķ�� �ش� ������ ����ó�� �ϴ� Req�� ������ �Լ� ȣ��.
	*/
	if (MailData->IsComplete() == false)
	{
		SendMail_Receive(MailData->GetMailUID());
	}
	else
	{
		SendMail_Delete(MailData->GetMailUID());
	}
}

#pragma endregion

// public.
//===============================================================================================================================================================================
// private.

#pragma region ItemWidget
bool UMail_Main::AddMail(const UMail_IteData* InMailInfo)
{
	if (InMailInfo == nullptr || InMailInfo->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("InMailInfo is valid false"));
		return;
	}

	if (InMailInfo->MailUID <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InMailInfo uid is 0"));
		return;
	}
	
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return;
	}

	if (ListView_MailList->GetNumItems() >= MAX_LOAD_COUNT)
	{
		// ���̻� ���� �� ����.
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is max count"));
		return;
	}

	//=========================================================================================================
	// add data.

	//	ListView�� ������ �߰�.
	ListView_MailList->AddItem(InMailInfo);
}

bool UMail_Main::ReceiveMail(const int64& InMailUID)
{
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return false;
	}

	if (MailItemList.Contains(InMailUID) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("MailItemList[%d] contains is false"), InMailUID);
		return false;
	}

	if (MailItemList[InMailUID] == nullptr || MailItemList[InMailUID]->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("MailItemList[%d] is valid false"), InMailUID);
		return false;
	}

	//=========================================================================================================
	// update data.
	
	//	�����͸� �Ϸ� ó�� ����.
	MailItemList[InMailUID]->SetComplete(true);

	//	ListView���� ���̰� �ִ� ������ ���� ����.
	UMail_Item* ViewItem = Cast<UMail_ListItem>(ListView_MailList->GetEntryWidgetFromItem(MailItemList[InMailUID]));
	if (ViewItem != nullptr && ViewItem->IsValidLowLevel() == true)
	{
		// view item�� ���� �� ����. ���� �ƴ�.
		ViewItem->UpdateMail();
	}
}

bool UMail_Main::RemoveMail(const int64& InMailUID)
{
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return false;
	}

	if (MailItemList.Contains(InMailUID) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("MailItemList[%d] contains is false"), InMailUID);
		return false;
	}

	//=========================================================================================================
	// view widget remove.

	//	ListView���� ���� �ش� ������ ���� ��Ŵ.
	ListView_MailList->RemoveItem(MailItemList[InMailUID]);

	//	���������� ������ ���� ó��.
	MailItemList.Remove(InMailUID);

	return true;
}

#pragma endregion

#pragma region UpdateItem
void UMail_Main::UpdateMail()
{
	//	����� �����͸� �̿��� ListView�� �����۵��� �߰�����.
	UpdateAllItem();
	
	//	���� ���� ǥ�� ����.
	UpdateMailCountUI();
}

void UMail_Main::UpdateMailCountUI()
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
	if (Text_MailCount == nullptr || Text_MailCount->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Text_MailCount is valid false"));
		return;
	}

	//=========================================================================================================
	//	������ �ϳ��� ������ ����ִٴ� �ؽ�Ʈ�� �����.
	CanvasPanel_Empty->SetVisibility((bool)(TotalCount <= 0) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	//	���� �� �ִ� ������ ������ Ȱ��ȭ.
	Bt_AllReceive->SetActive((bool)(ReceivableCount > 0));
	//	������ �� �ִ� ������ ������ Ȱ��ȭ.
	Bt_Delete->SetActive((bool)(ReceivableCount < TotalCount));

	//	��� ������ ���� ǥ��. MAX_LOAD_COUNT�� �̻��� ��� MAX_LOAD_COUNT �ڿ� +�� �ٿ���.
	Text_MailCount->SetText(TotalCount <= MAX_LOAD_COUNT ? FText::AsNumber(TotalCount) : FText::FromString(FString::Printf(TEXT("%d+"), MAX_LOAD_COUNT)));
}

void UMail_Main::UpdateAllItem()
{
	if (ListView_MailList == nullptr || ListView_MailList->IsValidLowLevel() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is valid false"));
		return;
	}

	//=========================================================================================================
	//	reset data.
	ReceivableCount = 0;
	TotalCount = MailItemList.Num();

	//	ListView�� �������� ��� ������.
	ListView_MailList->ClearListItems();

	for (const TPair<int64, UMail_IteData*>& MailInfo : MailItemList)
	{
		if (MailInfo.Value->IsComplete() == false)
		{
			//	������ ���� ���� ������ ���� ��� ������.
			ReceivableCount++;
		}

		//	ListView�� ������ �߰�.
		if (AddMail(MailInfo.Value) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AddMail is false [ Mail UID : %d ]"), MailInfo.Key);
			continue;
		}
	}

	//	�� �߰��� ������ ListView �ѹ� ���� ������.
	ListView_MailList->RegenerateAllEntries();
}

#pragma endregion

#pragma region Network_Send_Function
void UMail_Main::SendMail_List()
{
	//  TODO : network send.
}

void UMail_Main::SendMail_Receive(const int64& InMailUID)
{
	//  TODO : network send.
}

void UMail_Main::SendMail_Delete(const int64& InMailUID)
{
	//  TODO : network send.
}

void UMail_Main::SendMail_AllReceive()
{
	//  TODO : network send.
}

void UMail_Main::SendMail_AllDelete()
{
	//  TODO : network send.
}

#pragma endregion

#pragma region Button_Event
void UMail_Main::OnClicked_Bt_Close()
{
	//	ViewPort���� ���� UMG�� ������.
	this->RemoveToParent();
}

void UMail_Main::OnClicked_Bt_AllReceive()
{
	if (MailItemList.Num() <= 0 || ReceivableCount <= 0)
	{
		return;
	}

	//	��� ������ �޴� Req ȣ��.
	SendMail_AllReceive();
}

void UMail_Main::OnClicked_Bt_Delete()
{
	if (MailItemList.Num() <= 0 || TotalCount <= 0)
	{
		return;
	}

	//	������ ���� ��� ������ �����ϴ� Req ȣ��.
	SendMail_AllDelete();
}

#pragma endregion

#undef MAX_LOAD_COUNT
#undef LIMIT_CONTENT
