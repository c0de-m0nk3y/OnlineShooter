// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnShooter_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ONLINESHOOTER_API UOnShooter_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UOnShooter_GameInstance();

	virtual void OnCreateSessionComplete(FName ServerName, bool Succeeded);

protected:
	virtual void Init() override;

	

	IOnlineSessionPtr sessionInterface;

	UFUNCTION(BlueprintCallable)
		void CreateServer();

	UFUNCTION(BlueprintCallable)
		void JoinServer();


};
