#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemEpicPackage.h"
#include "OnlineSubsystemTypes.h"
#include "IPAddress.h"

class FUniqueNetIdEpic
	: public FUniqueNetIdString
{
public:
	FUniqueNetIdEpic()
		: FUniqueNetIdString()
	{
		Type = EPIC_SUBSYSTEM;
	}
	explicit FUniqueNetIdEpic(const FString& InUniqueNetId)
		: FUniqueNetIdString(InUniqueNetId, EPIC_SUBSYSTEM)
	{
	}
	explicit FUniqueNetIdEpic(FString&& InUniqueNetId)
		: FUniqueNetIdString(MoveTemp(InUniqueNetId), EPIC_SUBSYSTEM)
	{
	}
	explicit FUniqueNetIdEpic(const FUniqueNetId& Src)
		: FUniqueNetIdString(Src)
	{
		check(GetType() == EPIC_SUBSYSTEM);
	}
	friend uint32 GetTypeHash(const FUniqueNetIdEpic& A)
	{
		return ::GetTypeHash(A.UniqueNetIdStr);
	}
	static const TSharedRef<const FUniqueNetIdEpic>& EmptyId()
	{
		static const TSharedRef<const FUniqueNetIdEpic> EmptyId = MakeShared<const FUniqueNetIdEpic>(FString());
		return EmptyId;
	}
};

UENUM()
enum class ELoginType : uint8
{
	Password		UMETA(DisplayName = "Password"),
	ExchangeCode	UMETA(DisplayName = "Exchange Code"),
	DeviceCode		UMETA(DisplayName = "Device Code"),
	Developer		UMETA(DisplayName = "Developer"),
	RefreshToken	UMETA(DisplayName = "Refresh Token"),
	AccountPortal	UMETA(DisplayName = "Account Portal"),
	PersistentAuth	UMETA(DisplayName = "Persistent Auth"),
};


/**
 * User attribution constants for GetUserAttribute()
 */
#define USER_ATTR_COUNTRY TEXT("country")
#define USER_ATTR_PREFERRED_LANGUAGE TEXT("preferred_language")

 /**
  * Info associated with an user account generated by this online service
  */
class FUserOnlineAccountEpic :
	public FUserOnlineAccount
{
private:
	/** User Id represented as a FUniqueNetId */
	TSharedRef<const FUniqueNetId> UserIdPtr;

	/** Additional key/value pair data related to auth */
	TMap<FString, FString> AdditionalAuthData;

	/** Additional key/value pair data related to user attribution */
	TMap<FString, FString> UserAttributes;

public:
	virtual TSharedRef<const FUniqueNetId> GetUserId() const override;

	virtual FString GetRealName() const override;
	virtual FString GetDisplayName(const FString& Platform = FString()) const override;
	virtual FString GetAccessToken() const override;
	virtual bool GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const override;
	virtual bool SetUserAttribute(const FString& AttrName, const FString& AttrValue) override;
	virtual bool GetAuthAttribute(const FString& AttrName, FString& OutAttrValue) const override;
	virtual bool SetAuthAttribute(const FString& AttrName, const FString& AttrValue);

	FUserOnlineAccountEpic(TSharedRef<FUniqueNetId const> InUserIdPtr)
		: UserIdPtr(InUserIdPtr)
	{
	}

	FUserOnlineAccountEpic(const FString& InUserId = TEXT(""))
		: UserIdPtr(new FUniqueNetIdEpic(InUserId))
	{
	}

	virtual ~FUserOnlineAccountEpic() = default;
};

/**
 * Epic implementation of session information
 */
class FOnlineSessionInfoEpic : public FOnlineSessionInfo
{
protected:

	/** Hidden on purpose */
	FOnlineSessionInfoEpic(const FOnlineSessionInfoEpic& Src)
	{
	}

	/** Hidden on purpose */
	FOnlineSessionInfoEpic& operator=(const FOnlineSessionInfoEpic& Src)
	{
		return *this;
	}

PACKAGE_SCOPE:

	/** Constructor */
	FOnlineSessionInfoEpic();

	/** The ip & port that the host is listening on (valid for LAN/GameServer) */
	TSharedPtr<class FInternetAddr> HostAddr;

	/** Unique Id for this session */
	TSharedPtr<FUniqueNetId> SessionId;

public:

	virtual ~FOnlineSessionInfoEpic() {}

	bool operator==(const FOnlineSessionInfoEpic& Other) const
	{
		return false;
	}

	virtual const uint8* GetBytes() const override
	{
		return nullptr;
	}

	virtual int32 GetSize() const override
	{
		return sizeof(uint64) + sizeof(TSharedPtr<class FInternetAddr>);
	}

	virtual bool IsValid() const override
	{
		// LAN case
		return HostAddr.IsValid() && HostAddr->IsValid();
	}

	virtual FString ToString() const override
	{
		return SessionId->ToString();
	}

	virtual FString ToDebugString() const override
	{
		return FString::Printf(TEXT("HostIP: %s SessionId: %s"),
			HostAddr.IsValid() ? *HostAddr->ToString(true) : TEXT("INVALID"),
			*SessionId->ToDebugString());
	}

	virtual const FUniqueNetId& GetSessionId() const override
	{
		return *SessionId;
	}
};
