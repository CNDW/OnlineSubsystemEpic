#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineUserInterface.h"
#include "eos_sdk.h"
#include "Misc/ScopeLock.h"
#include "OnlineSubsystemEpicPackage.h" // Needs to be the last include

class FOnlineSubsystemEpic;

class FOnlineUserEpic
	: public IOnlineUser
{
private:
	/** Hidden on purpose */
	FOnlineUserEpic()
		: Subsystem(nullptr)
	{
	}

	/** The subsystem that owns this instance*/
	FOnlineSubsystemEpic* Subsystem;

	EOS_HUserInfo userInfoHandle;

	/**
	 * Concatenates multiple error strings into one single error string.
	 * @param ErrorStrings - The errors to concatenate
	 * @returns - A single string containing all errors separated by a ';'
	 */
	FString ConcatErrorString(TArray<FString> ErrorStrings);

	/** Since the UserIdMappings callbacks are pretty much identical, this can be done without problems */
	void HandleQueryUserIdMappingsCallback(FOnlineUserEpic* ThisPtr, EOS_EResult Result, EOS_EpicAccountId LocalUserId, EOS_EpicAccountId TargetUserId, double StartTime, FOnQueryExternalIdMappingsComplete const& CompletionDelegate);

	/**
	 * A list of all running user queries
	 * @key - The start time of the query
	 * @value - A tuple containing data with the queried id, the query state, and the optional error message
	 */
	TMap<double, TTuple<TArray<TSharedRef<FUniqueNetId const>>, TArray<bool>, TArray<FString>>> UserQueries;

	/**
	 * A list of all currently running external id mappings queries
	 * @key - The start time of the query
	 * @value - A tuple containing the data with the queried data (either id or display name), the state and optional error message
	 */
	TMap<double, TTuple<FExternalIdQueryOptions, TArray<FString>, TArray<bool>, TArray<FString>>> ExternalIdMappingsQueries;

	/** A list of all user ids for which the SDK has cached user information. */
	TArray<TSharedRef<FUniqueNetId const>> queriedUserIds;

	/**
	 * A list of all cached id mappings.
	 * @note - This is necessary as the EOS SDK wants a local user id 
	 * to retrieve cached information about an external id mapping.
	 * However the methods to retrieve the data don't offer a way
	 * to specify the user id that wants to retrieve the cached data.
	 */
	TArray<struct FExternalIdMapping> externalIdMappings;

	static void OnEOSQueryUserInfoComplete(EOS_UserInfo_QueryUserInfoCallbackInfo const* Data);
	static void OnEOSQueryUserInfoByDisplayNameComplete(EOS_UserInfo_QueryUserInfoByDisplayNameCallbackInfo const* Data);
	static void OnEOSQueryExternalIdMappingsByDisplayNameComplete(EOS_UserInfo_QueryUserInfoByDisplayNameCallbackInfo const* Data);
	static void OnEOSQueryExternalIdMappingsByIdComplete(EOS_UserInfo_QueryUserInfoCallbackInfo const* Data);

PACKAGE_SCOPE:
	/** Critical sections for thread safe operation of user query lists */
	mutable FCriticalSection UserQueryLock;

	/** Critical sections for thread safe operation of external id mappings lists */
	mutable FCriticalSection ExternalIdMappingsQueriesLock;

	/**
	 * Creates a new instance of the FOnlineSessionEpic class.
	 * @ InSubsystem - The subsystem that owns the instance.
	 */
	FOnlineUserEpic(FOnlineSubsystemEpic* InSubsystem);

	/** Session tick for various background */
	void Tick(float DeltaTime);

public:

	// IOnlineUser
	virtual bool QueryUserInfo(int32 LocalUserNum, const TArray<TSharedRef<const FUniqueNetId> >& UserIds) override;
	virtual bool GetAllUserInfo(int32 LocalUserNum, TArray< TSharedRef<class FOnlineUser> >& OutUsers) override;
	virtual TSharedPtr<FOnlineUser> GetUserInfo(int32 LocalUserNum, const class FUniqueNetId& UserId) override;
	virtual bool QueryUserIdMapping(const FUniqueNetId& UserId, const FString& DisplayNameOrEmail, const FOnQueryUserMappingComplete& Delegate = FOnQueryUserMappingComplete()) override;
	virtual bool QueryExternalIdMappings(const FUniqueNetId& UserId, const FExternalIdQueryOptions& QueryOptions, const TArray<FString>& ExternalIds, const FOnQueryExternalIdMappingsComplete& Delegate = FOnQueryExternalIdMappingsComplete()) override;
	virtual void GetExternalIdMappings(const FExternalIdQueryOptions& QueryOptions, const TArray<FString>& ExternalIds, TArray<TSharedPtr<const FUniqueNetId>>& OutIds) override;
	virtual TSharedPtr<const FUniqueNetId> GetExternalIdMapping(const FExternalIdQueryOptions& QueryOptions, const FString& ExternalId) override;
};
using FOnlineUserEpicPtr = TSharedPtr<FOnlineUserEpic, ESPMode::ThreadSafe>;