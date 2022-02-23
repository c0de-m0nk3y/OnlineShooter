// Fill out your copyright notice in the Description page of Project Settings.


#include "OnShooter_GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

const static FName SESSION_NAME = TEXT("Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UOnShooter_GameInstance::UOnShooter_GameInstance()
{

}

void UOnShooter_GameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("INIT"));
	subSystem = IOnlineSubsystem::Get();
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
		sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UOnShooter_GameInstance::OnDestroySessionComplete);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UOnShooter_GameInstance::OnFindSessionsComplete);
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UOnShooter_GameInstance::OnJoinSessionComplete);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: sessionInterface was not valid"));
		return;
	}
}

void UOnShooter_GameInstance::OnCreateSessionComplete(FName ServerName, bool success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete success=%d"), success);
	if (success)
	{
		GetWorld()->ServerTravel("/Game/Scenes/dev_scene?listen");
	}
}

void UOnShooter_GameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
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

	TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults;
	UE_LOG(LogTemp, Warning, TEXT("Sessions found=%d"), searchResults.Num());
	if (searchResults.Num() != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("attempting to join existing session"));
		sessionInterface->JoinSession(0, SESSION_NAME, searchResults[0]);
	}
	
}

void UOnShooter_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("JoinSession complete: %s"), *SessionName.ToString());
	
	if (Result == EOnJoinSessionCompleteResult::Type::Success)
		UE_LOG(LogTemp, Warning, TEXT("JOIN OK"));
	if (Result != EOnJoinSessionCompleteResult::Type::Success)
		UE_LOG(LogTemp, Warning, TEXT("JOIN NOT OK"));

	
	if (APlayerController* playerController = GetFirstLocalPlayerController())
	{
		FString join_address;
		if (sessionInterface->GetResolvedConnectString(SessionName, join_address))
		{
			UE_LOG(LogTemp, Warning, TEXT("joining SERVER: %s"), *join_address);
			playerController->ClientTravel(join_address, ETravelType::TRAVEL_Absolute);
		}		
	}
}

void UOnShooter_GameInstance::CreateSession()
{
	UE_LOG(LogTemp, Warning, TEXT("CreateServer"));

	FOnlineSessionSettings s;
	s.bAllowInvites = true;
	s.bAllowJoinInProgress = true;
	s.bAllowJoinViaPresence = true;
	s.bIsDedicated = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		UE_LOG(LogTemp, Warning, TEXT("SUBSYSTEM WAS 'NULL'"));
		s.bIsLANMatch = true;
	}	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SUBSYSTEM WAS NOT NULL"));
		s.bIsLANMatch = false;
	}
		
	s.NumPublicConnections = 5;
	s.bShouldAdvertise = true;
	s.bUsesPresence = true;
	s.bUseLobbiesIfAvailable = true;

	sessionInterface->CreateSession(0, FName("My Session"), s);
}

void UOnShooter_GameInstance::Button_HostServer()
{
	ServerNameLabel = "the_only_server";
	if (sessionInterface.IsValid())
	{
		auto ExistingSession = sessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			sessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}

void UOnShooter_GameInstance::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Join server clicked"));

	
	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	//sessionSearch->bIsLanQuery = true; //LAN
	sessionSearch->MaxSearchResults = 10000;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
	
}
