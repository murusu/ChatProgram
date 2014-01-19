#ifndef USER

#define USER

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>


#define LOGIN_REQUEST         101
#define LOGIN_SUCCESS         102
#define LOGIN_FAIL            103
#define LOGOUT_REQUEST        104
#define LOGOUT_SUCCESS        105
#define LOGOUT_FAIL           106
#define RESGISTER_REQUEST     107
#define RESGISTER_SUCCESS     108
#define RESGISTER_FAIL        109
#define CREATEGROUP_REQUEST   110
#define CREATEGROUP_SUCCESS   111
#define CREATEGROUP_FAIL      112
#define DESTROYGROUP_REQUEST  113
#define DESTROYGROUP_SUCCESS  114
#define DESTROYGROUP_FAIL     115
#define ADDUSER               116
#define REMOVEUSER            117
#define ADDGROUP              118
#define REMOVEGROUP           119

#define JOINGROUP_REQUEST     120
#define JOINGROUP_SUCCESS     121
#define JOINGROUP_FAIL        122
#define ADDGROUPMEMBER        123 
#define LEFTGROUP_REQUEST     124
#define LEFTGROUP_SUCCESS     125
#define LEFTGROUP_FAIL        126
#define REMOVEGROUPMEMBER     127     
#define MESSAGE               128

#define READING               129
#define IDLE                  130
#define LISTENSOCKET          131

#define ONLINE                1
#define OFFLINE               0

#define MAX_USER              64
#define MAX_GROUP             64
#define MAX_SOCKET            MAX_USER+10
#define MAX_BUFFER            64000
#define INFO_LENGTH           14
#define EMPTYCONTENT          255

#define TOUSER                1
#define TOGROUP               2
#define TOSERVER              3               


struct stUser
{
    struct sockaddr_in Useraddr;  
	char   szUserName[INFO_LENGTH];
	char   szUserPassWord[INFO_LENGTH];
	int    iUserID;
	int    iSocketNO;
	int    iIndex;
};

struct stGroup
{
	char   szGroupName[INFO_LENGTH];
	char   szGroupPassWord[INFO_LENGTH];
	int    iGroupID;
	int    iOwnerID;
	int    iIndex;
};

struct stUserServer
{
    struct sockaddr_in Useraddr;  
	char   szUserName[INFO_LENGTH];
	int    iUserID;
};

struct stGroupServer
{
	char   szGroupName[INFO_LENGTH];
	int    iGroupID;
	int    iOwnerID;
};

struct stSocketInfo
{
	int	   iInfo;
	SOCKET socket;
	char   *szTempBuffer;
	int    iMessageLength;
	int    iReceived;
};

struct stTransMessage
{
	int    iMessageLength;
	char   szSender[32];
	int    iMessageType;
	char   szAddMessage[MAX_BUFFER - 40];
};


class UserObject
{
	private:

	public:
		bool   bServerOnLine;
		int    iRegisterUser;
		int    iOnlineUser;
		int    iGroupNumber;
		int    iMaxTextLength;

		struct stUser stUser[MAX_USER];
		struct stGroup stGroup[MAX_GROUP];
		struct stSocketInfo stSocketInfo[MAX_SOCKET];

		void vInitSocketInfo(void);
		void vInitUserList(void);
		void vInitGroupList(void);
		
        int  iGetSocket(void);
		
		void vCleanSocket(int iSocketNo);
		int  iFindUserFromSocketNo(int iSocketNo);
		int  iFindUserFromAddr(char * szIPAddr);
		int  iFindUserFromUserID(int iUserID);
		int  iFindGroupFromGroupID(int iGroupID);

		UserObject();
		~UserObject();

};


class MessageObject
{
	private:

	public:

        struct stTransMessage stTransMessage;
		char szMessageBuffer[MAX_BUFFER];

        void vCleanMessageBuffer(void);
		void vGetTransMessage(void);
		void vSetTransMessage(void);

		MessageObject();
		~MessageObject();

};



#endif
