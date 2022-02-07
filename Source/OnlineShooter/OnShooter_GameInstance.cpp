// Fill out your copyright notice in the Description page of Project Settings.


#include "OnShooter_GameInstance.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"


UOnShooter_GameInstance::UOnShooter_GameInstance()
{

}

void UOnShooter_GameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("INIT"));
	IOnlineSubsystem* subSystem = IOnlineSubsystem::Get();
	if (subSystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: subSystem was nullptr"));
		return;
	}

	sessionInterface = subSystem->GetSessionInterface();
	if (sessionInterface.IsValid())
	{
		//Bind delegates
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UOnShooter_GameInstance::OnCreateSessionComplete);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UOnShooter_GameInstance::OnFindSessionsComplete);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: sessionInterface was not valid"));
		return;
	}
}

void UOnShooter_GameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete success=%d"), Succeeded);
	if (Succeeded)
	{
		GetWorld()->ServerTravel("/Game/Scenes/dev_scene?listen", true);
	}
}

void UOnShooter_GameInstance::OnFindSessionsComplete(bool success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete success=%d"), success);
	if (!success)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete failed"));
		return;
	}

	int results_count = sessionSearch->SearchResults.Num();
	UE_LOG(LogTemp, Warning, TEXT("Servers found=%d"), results_count);
}

void UOnShooter_GameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Server now"));

	FOnlineSessionSettings s;
	s.bAllowJoinInProgress = true;
	s.bIsDedicated = false;
	s.bIsLANMatch = true; //LAN
	s.bShouldAdvertise = true;
	s.bUsesPresence = true;
	s.NumPublicConnections = 25;

	sessionInterface->CreateSession(0, FName("My Session"), s);
}

void UOnShooter_GameInstance::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Join server clicked"));

	
	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	sessionSearch->bIsLanQuery = true; //LAN
	sessionSearch->MaxSearchResults = 10000;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
	
}
