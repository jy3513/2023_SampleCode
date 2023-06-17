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
	//	임시 데이터 저장.
	MailItemList.Empty();
	MailItemList.Append(InMailList);

	//	데이터와 UI 모두 갱신.
	UpdateMail();
}

void UMail_Main::Callback_MailReceive(const int64& InMailUID)
{
	//	보상을 받았으니 ReceivableCount에서 빼줌.
	ReceivableCount--;

	//	데이터를 보상 받은 것으로 수정 한 후 해당 메일의 UI를 갱신 시켜줌.
	ReceiveMail(InMailUID);
	
	//	ReceivableCount가 변경되었으니 UI를 갱신 시켜줌.
	UpdateMailCountUI();
}

void UMail_Main::Callback_MailDelete(const int64& InMailUID)
{
	//	메일이 삭제 되었으니 TotalCount에서 빼줌.
	TotalCount--;

	//	데이터 삭제 처리와 해당 메일의 UI를 삭제 처리함.
	RemoveMail(InMailUID);

	//	TotalCount가 변경 되었으니 UI를 갱신 시켜줌.
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
	* 메일의 보상을 받았는지 확인.
	* 1. 보상을 받지 않았으면 메일 보상 받는 Req를 보내는 함수 호출.
	* 2. 보상을 받은 후라면 해당 메일을 삭제처리 하는 Req를 보내는 함수 호출.
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
		// 더이상 넣을 수 없음.
		UE_LOG(LogTemp, Warning, TEXT("ListView_MailList is max count"));
		return;
	}

	//=========================================================================================================
	// add data.

	//	ListView에 아이템 추가.
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
	
	//	데이터를 완료 처리 해줌.
	MailItemList[InMailUID]->SetComplete(true);

	//	ListView에서 보이고 있는 위젯을 갱신 해줌.
	UMail_Item* ViewItem = Cast<UMail_ListItem>(ListView_MailList->GetEntryWidgetFromItem(MailItemList[InMailUID]));
	if (ViewItem != nullptr && ViewItem->IsValidLowLevel() == true)
	{
		// view item이 없을 수 있음. 에러 아님.
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

	//	ListView에서 먼저 해당 메일을 삭제 시킴.
	ListView_MailList->RemoveItem(MailItemList[InMailUID]);

	//	마지막으로 데이터 삭제 처리.
	MailItemList.Remove(InMailUID);

	return true;
}

#pragma endregion

#pragma region UpdateItem
void UMail_Main::UpdateMail()
{
	//	저장된 데이터를 이용해 ListView에 아이템들을 추가해줌.
	UpdateAllItem();
	
	//	메일 갯수 표시 갱신.
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
	//	메일이 하나도 없으면 비어있다는 텍스트를 띄워줌.
	CanvasPanel_Empty->SetVisibility((bool)(TotalCount <= 0) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	//	받을 수 있는 메일이 있으면 활성화.
	Bt_AllReceive->SetActive((bool)(ReceivableCount > 0));
	//	삭제할 수 있는 메일이 있으면 활성화.
	Bt_Delete->SetActive((bool)(ReceivableCount < TotalCount));

	//	모든 메일의 갯수 표시. MAX_LOAD_COUNT개 이상일 경우 MAX_LOAD_COUNT 뒤에 +를 붙여줌.
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

	//	ListView의 아이템을 모두 지워줌.
	ListView_MailList->ClearListItems();

	for (const TPair<int64, UMail_IteData*>& MailInfo : MailItemList)
	{
		if (MailInfo.Value->IsComplete() == false)
		{
			//	보상을 받지 않은 메일이 있을 경우 더해줌.
			ReceivableCount++;
		}

		//	ListView에 아이템 추가.
		if (AddMail(MailInfo.Value) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AddMail is false [ Mail UID : %d ]"), MailInfo.Key);
			continue;
		}
	}

	//	다 추가한 다음에 ListView 한번 갱신 시켜줌.
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
	//	ViewPort에서 메일 UMG를 제거함.
	this->RemoveToParent();
}

void UMail_Main::OnClicked_Bt_AllReceive()
{
	if (MailItemList.Num() <= 0 || ReceivableCount <= 0)
	{
		return;
	}

	//	모든 메일을 받는 Req 호출.
	SendMail_AllReceive();
}

void UMail_Main::OnClicked_Bt_Delete()
{
	if (MailItemList.Num() <= 0 || TotalCount <= 0)
	{
		return;
	}

	//	보상을 받은 모든 메일을 삭제하는 Req 호출.
	SendMail_AllDelete();
}

#pragma endregion

#undef MAX_LOAD_COUNT
#undef LIMIT_CONTENT
