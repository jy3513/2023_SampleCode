// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Object.h"
#include "CharacterDataInventory.generated.h"

/**
 *
 */

class UCharacterData;

UCLASS()
class UCharacterDataInventory : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Release() override;
	virtual void Clear() override;

public:
	UCharacterData* AddHero(const int64& InItemUID, const int32& InDataID) override;

public:
	UCharacterData* GetHeroByUID(const int64& InItemUID) const;
	
	UCharacterData* GetHeroByDataID(const int32& InDataID) const;

	const bool GetAllHeros(TArray<UCharacterData*>& OutHeroList) const;

private:
	UPROPERTY(Transient)
		TArray<UCharacterData*> m_HeroList;
};

#pragma once