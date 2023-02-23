// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"

#include "Mail_Main.generated.h"

/**
 * ������ - ����.
 */

class UCanvasPanel;
class UTextBlock;
class UListView;
class UHorizontalBox;
class UScrollBox;

/**
 * ���� �����ʹ� Ŭ�� �����ϰ� ���� �ʾƼ� ���� �ӽ� ���� ������.
 * �ش� �����͸� UListView�� ä��.
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
 * ���� ���� UMG.
 */
UCLASS()
class UMail_Main : public UObject
{
	GENERATED_BODY()

public:
	/**
	* ó�� ���� UMG ���� �� ȣ�� �Ǵ� �Լ�.
	* ��ư�� �Լ��� ���� �ϰ� ���� ����Ʈ�� �޴� Req�� ������ ����.
	*/
	void Init();


public:
#pragma region Callback_Function
	/**
	* �����κ��� ���� ����Ʈ Recv�� ���� �� ȣ��Ǵ� �Լ�.
	* InMailList : ���� Ŭ������ ������ ���� �ӽ� ������.
	*/
	void Callback_MailList(const TArray<UMail_Item_Data*>& InMailList);
	
	/**
	* ���� ���� Recv�� ���� �� ȣ�� �Ǵ� �Լ�.
	* InMailUID : ������ ���� ������ UID.
	*/
	void Callback_MailReceive(const int64& InMailUID);

	/**
	* ���� ���� Recv�� ���� �� ȣ�� �Ǵ� �Լ�.
	* InMailUID : ���� ó�� �� ������ UID.
	*/
	void Callback_MailDelete(const int64& InMailUID);

	/**
	* ���� �������� ��ư�� ������ ȣ�� �Ǵ� �Լ�.
	* ������ ���� �ʾ����� ���� �ޱ�, ������ �޾����� ���� ó�� ���ִ� ���.
	* InMailUID : ������ �ްų� ���� ó���� ������ UObject.
	*/
	void Callback_MailButton(UObject* InMailObject);


#pragma endregion

private:
#pragma region ItemWidget
	/**
	* ���� ����Ʈ�� ���� �߰� �� �� ������ ����.
	* InMailInfo : ���� �߰��� ����.
	*/
	bool AddMail(const UMail_Item_Data* InMailInfo);

	/**
	* �������� ������ ���� ���� UMG���� �ش� ������ UI�� �����͸� ���� ������.
	* InMailUID : ������ ���� ������ UID.
	*/
	bool ReceiveMail(const int64& InMailUID);

	/**
	* �������� ���� ó���� ���� UMG���� �ش� ������ UI�� �����͸� ������.
	* InMailUID : ���� ó�� �� ������ UID.
	*/
	bool RemoveMail(const int64& InMailUID);


#pragma endregion

#pragma region UpdateItem
	/**
	* ���� ����Ʈ Req ���� �� ȣ���.
	* �����ϰ� �ִ� �����Ϳ� ���� ��� UI ����.
	*/
	void UpdateMail();

	/**
	* ���� ������ Ȯ�� �� �� UI �ٲ���.
	* 1. ���� ������ Ȯ�� �� �� ��� �ִٴ� �ؽ�Ʈ�� �����.
	* 2. ��� �ޱ�, ��� ���� ��ư Ȱ��ȭ ���θ� ������.
	* 3. ���� ���� ǥ��.
	*/
	void UpdateMailCountUI();

	/**
	* ���� ���� �����͸� ��� ListView�� �߰�.
	*/
	void UpdateAllItem();


#pragma endregion

#pragma region Network_Send_Function
	/**
	* ������ ���� ����Ʈ Req�� ����.
	*/
	void SendMail_List();

	/**
	* ������ ������ ���� ������ UID�� ����.
	* InMailUID : ������ ���� ������ UID.
	*/
	void SendMail_Receive(const int64& InMailUID);

	/**
	* ������ ������ ������ UID�� ����.
	* InMailUID : ������ ������ UID.
	*/
	void SendMail_Delete(const int64& InMailUID);

	/**
	* ������ ��� ������ ������ �޴� Req�� ����.
	*/
	void SendMail_AllReceive();

	/**
	* ������ ������ ���� ��� ������ �����ϴ� Req�� ����.
	*/
	void SendMail_AllDelete();


#pragma endregion

#pragma region Button_Event
private:
	/**
	* �ݱ� ��ư�� ������ �� ȣ�� ��.
	* ���� UMG�� ViewPort���� ������.
	*/
	UFUNCTION() void OnClicked_Bt_Close();

	/**
	* ��� �ޱ� ��ư�� ������ �� ȣ�� ��.
	* ����ó�� �� ������ Req������ �Լ� ȣ�� ��.
	*/
	UFUNCTION() void OnClicked_Bt_AllReceive();

	/**
	* ��� ���� ��ư�� ������ �� ȣ�� ��.
	* ����ó�� �� ������ Req������ �Լ� ȣ�� ��.
	*/
	UFUNCTION() void OnClicked_Bt_Delete();


#pragma endregion


protected:
	//	���� �����Կ� �ִ� ��� ������ ����. �ִ� ������ �Ѿ �� MAX_LOAD_COUNT�� ��� ǥ��.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UTextBlock* Text_MailCount = nullptr;
	//	���� �����Ͱ� �� UListView.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UListView* ListView_MailList = nullptr;
	//	���� ������ �ϳ��� ���� �� ������ �ϴ� ����.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UCanvasPanel* CanvasPanel_Empty = nullptr;
	//	���� UMG�� ����.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Close = nullptr;
	//	��� ������ ���� �ޱ� ��ư.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_AllReceive = nullptr;
	//	������ ���� ������ �����ϴ� ��ư.
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) UButton* Bt_Delete = nullptr;


private:
	//	������ ���� ���� �� �ִ� ������ ���� ����.
	int32 ReceivableCount = INDEX_NONE;
	//	��� ������ ����. ���� ���� + ���� ����.
	int32 TotalCount = INDEX_NONE;

	//	�������� �޾Ƽ� �ӽ� �����ϰ� �ִ� ���� ������ ����Ʈ.
	UPROPERTY() TMap<int64, UMail_Item_Data*> MailItemList;
};
