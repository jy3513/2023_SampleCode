// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"

#include "Mail_Main.generated.h"

/**
 * 컨텐츠 - 메일.
 */

class UCanvasPanel;
class UTextBlock;
class UListView;
class UHorizontalBox;
class UScrollBox;

/**
 * 메일 데이터는 클라에 저장하고 있지 않아서 만든 임시 메일 데이터.
 * 해당 데이터를 UListView를 채움.
 */
UCLASS()
class UMail_Item_Data : public UObject
{
	GENERATED_BODY()

public:
	void SetMailUID(const int64& InMailUID) { MailUID = InMailUID; }
	void SetMailContext(const FText& InContext) { MailContext = InContext; }
	void SetComplete(const bool InbComplete) { IsComplete = InbComplete; }

public:
	const int64& GetMailUID() const { return MailUID; }
	const FText& GetMailContext() const { return MailContext; }
	const bool IsComplete() const { return IsComplete; }

private:
	int64 MailUID = 0;
	FText MailContext = FText("");
	bool IsComplete = false;
};

/**
 * 메일 메인 UMG.
 */
UCLASS()
class UMail_Main : public UObject
{
	GENERATED_BODY()

public:
	/**
	* 처음 메일 UMG 진입 시 호출 되는 함수.
	* 버튼을 함수와 연결 하고 메일 리스트를 받는 Req를 서버로 보냄.
	*/
	void Init();


public:
#pragma region Callback_Function
	/**
	* 서버로부터 메일 리스트 Recv를 받은 후 호출되는 함수.
	* InMailList : 현재 클래스에 저장할 메일 임시 데이터.
	*/
	void Callback_MailList(const TArray<UMail_Item_Data*>& InMailList);
	
	/**
	* 메일 보상 Recv를 받은 후 호출 되는 함수.
	* InMailUID : 보상을 받은 메일의 UID.
	*/
	void Callback_MailReceive(const int64& InMailUID);

	/**
	* 메일 삭제 Recv를 받은 후 호출 되는 함수.
	* InMailUID : 삭제 처리 된 메일의 UID.
	*/
	void Callback_MailDelete(const int64& InMailUID);

	/**
	* 메일 아이템의 버튼을 누르면 호출 되는 함수.
	* 보상을 받지 않았으면 보상 받기, 보상을 받았으면 삭제 처리 해주는 기능.
	* InMailUID : 보상을 받거나 삭제 처리할 메일의 UObject.
	*/
	void Callback_MailButton(UObject* InMailObject);


#pragma endregion

private:
#pragma region ItemWidget
	/**
	* 메일 리스트에 새로 추가 해 줄 메일의 정보.
	* InMailInfo : 새로 추가할 정보.
	*/
	bool AddMail(const UMail_Item_Data* InMailInfo);

	/**
	* 서버에서 보상을 받은 이후 UMG에서 해당 메일의 UI와 데이터를 갱신 시켜줌.
	* InMailUID : 보상을 받은 메일의 UID.
	*/
	bool ReceiveMail(const int64& InMailUID);

	/**
	* 서버에서 삭제 처리한 이후 UMG에서 해당 메일의 UI와 데이터를 지워줌.
	* InMailUID : 삭제 처리 된 메일의 UID.
	*/
	bool RemoveMail(const int64& InMailUID);


#pragma endregion

#pragma region UpdateItem
	/**
	* 메일 리스트 Req 받은 후 호출됨.
	* 저장하고 있는 데이터에 맞춰 모든 UI 갱신.
	*/
	void UpdateMail();

	/**
	* 메일 갯수를 확인 한 후 UI 바꿔줌.
	* 1. 메일 갯수를 확인 한 후 비어 있다는 텍스트를 띄워줌.
	* 2. 모두 받기, 모두 삭제 버튼 활성화 여부를 결정함.
	* 3. 메일 갯수 표시.
	*/
	void UpdateMailCountUI();

	/**
	* 저장 중인 데이터를 모두 ListView에 추가.
	*/
	void UpdateAllItem();


#pragma endregion

#pragma region Network_Send_Function
	/**
	* 서버로 메일 리스트 Req를 보냄.
	*/
	void SendMail_List();

	/**
	* 서버로 보상을 받을 메일의 UID를 보냄.
	* InMailUID : 보상을 받을 메일의 UID.
	*/
	void SendMail_Receive(const int64& InMailUID);

	/**
	* 서버로 삭제할 메일의 UID를 보냄.
	* InMailUID : 삭제할 메일의 UID.
	*/
	void SendMail_Delete(const int64& InMailUID);

	/**
	* 서버로 모든 메일의 보상을 받는 Req를 보냄.
	*/
	void SendMail_AllReceive();

	/**
	* 서버로 보상을 받은 모든 메일을 삭제하는 Req를 보냄.
	*/
	void SendMail_AllDelete();


#pragma endregion

#pragma region Button_Event
private:
	/**
	* 닫기 버튼을 눌렀을 때 호출 됨.
	* 메일 UMG를 ViewPort에서 제거함.
	*/
	UFUNCTION() void OnClicked_Bt_Close();

	/**
	* 모두 받기 버튼을 눌렀을 때 호출 됨.
	* 예외처리 후 서버로 Req보내는 함수 호출 함.
	*/
	UFUNCTION() void OnClicked_Bt_AllReceive();

	/**
	* 모두 삭제 버튼을 눌렀을 때 호출 됨.
	* 예외처리 후 서버로 Req보내는 함수 호출 함.
	*/
	UFUNCTION() void OnClicked_Bt_Delete();


#pragma endregion


protected:
	//	현재 메일함에 있는 모든 메일의 갯수. 최대 갯수가 넘어갈 시 MAX_LOAD_COUNT로 대신 표시.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTextBlock* Text_MailCount = nullptr;
	//	메일 데이터가 들어갈 UListView.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UListView* ListView_MailList = nullptr;
	//	받은 메일이 하나도 없을 시 띄워줘야 하는 위젯.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UCanvasPanel* CanvasPanel_Empty = nullptr;
	//	메일 UMG를 닫음.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Close = nullptr;
	//	모든 메일의 보상 받기 버튼.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_AllReceive = nullptr;
	//	보상을 받은 메일을 삭제하는 버튼.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Delete = nullptr;


private:
	//	보상을 아직 받을 수 있는 상태의 메일 갯수.
	int32 ReceivableCount = INDEX_NONE;
	//	모든 메일의 갯수. 보상 받음 + 받지 않음.
	int32 TotalCount = INDEX_NONE;

	//	서버에서 받아서 임시 저장하고 있는 메일 데이터 리스트.
	UPROPERTY() TMap<int64, UMail_Item_Data*> MailItemList;
};
