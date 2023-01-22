// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "client_R.h"
#include "Object.h"
#include "CoreMinimal.h"
#include "Mail_Main.generated.h"

/**
 *
 */

class UCanvasPanel;
class UTextBlock;
class UListView;
class UHorizontalBox;
class UScrollBox;

class UMail_Item_Data;

UCLASS()
class CLIENT_R_API UMail_Main : public UObject
{
	GENERATED_BODY()

public:
	void Init() override;
	void EventConnect() override;

public:
	void CallbackMailList(const int32& InMailTotalCount, const TArray<UMail_Item_Data*>& InMailList);
	void CallbackMailReceive(const int64& InMailUID, const bool IsGetReceive);
	void CallbackMailDelete(const int64& InMailUID);

private:
	void SetUp();

private:
#pragma region ItemWidget
	// remove.
	bool RemoveMail(const int64& InMailUID);
	// listview 위젯에 저장된 데이터들을 pool에 저장한 후 지워줌.
	void ClearListItems();
	// item widget.
	void AddMail(const UMail_Item_Data* InMailInfo);
	// callback mail button.
	void CallbackMail(const int64& InMailUID);
#pragma endregion

#pragma region UpdateItem
	// update mail.
	void UpdateMail();
	// update ui.
	void UpdateMailUI();
	//
	void UpdateAllItem();
#pragma endregion

#pragma region Mail_UMG
	// show empty ui.
	void CheckEmptyUI();
	// set mail count.
	void SetMailCount();
#pragma endregion

#pragma region Network_Send_Function
	void SendMailList();
	void SendMailReceive(const int64& InMailUID);
	void SendMailDelete(const int64& InMailUID);
	void SendMailAllReceive();
	void SendMailAllDelete();
#pragma endregion

#pragma region Button_Event
private:
	UFUNCTION() void OnClicked_Bt_Close();
	UFUNCTION() void OnClicked_Bt_AllReceive();
	UFUNCTION() void OnClicked_Bt_Delete();
#pragma endregion

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTextBlock* Text_Receive_Num = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Close = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UListView* ListView_MailList = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UCanvasPanel* CanvasPanel_Empty = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_AllReceive = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Delete = nullptr;

private:
	int32 m_ReceivableCount = INDEX_NONE;
	int32 m_TotalCount = INDEX_NONE;

	UPROPERTY() TMap<int64, UMail_Item_Data*> m_MailItemList;
};
