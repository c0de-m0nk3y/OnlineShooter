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

	

protected:
	virtual void Init();

	

	IOnlineSessionPtr sessionInterface;
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	void OnCreateSessionComplete(FName ServerName, bool Succeeded);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void CreateSession();

	FString ServerNameLabel;

	UFUNCTION(BlueprintCallable)
		void Button_HostServer();

	UFUNCTION(BlueprintCallable)
		void JoinServer();

	class IOnlineSubsystem* subSystem;

};
