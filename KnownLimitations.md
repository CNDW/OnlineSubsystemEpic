# Known Limitations
* Session searches by user ID only search for local user ids
* LAN Session searches are not supported
* After a session search has completed the ping towards the server will always be -1
* The OnlineUser Interface might not fill out every possible field for non owned users. From the EOS SDK documentation:
    > Most of the information in the EOS_UserInfo structure will be empty for non-local users. This is to ensure that EOS does not provide personally identifiable information (PII) to other users. The DisplayName and UserId fields are the only ones the EOS SDK guarantees to populate.
* OnlineUser::GetUserInfo() retrieves the preferred nickname for the requested user properly, but the underlying user type doesn't store it just yet.