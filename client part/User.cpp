#include "User.h"

// Constructor
UserObject::UserObject()
{
	vInitUserList();

	vInitGroupList();

	vInitServer();

	vInitHost();

	vInitSocketInfo();

	bUserOnLine = false;

	iMaxTextLength = 0;
}

// Destructor
UserObject::~UserObject()
{
}


void UserObject::vGetServerInfo (HWND hIP, HWND hPort)
{
	char        szServerIP[32];
	char        szServerPort[8];

   	GetWindowText(hIP,szServerIP,32);
	GetWindowText(hPort,szServerPort,8);

	stServer.Serveraddr.sin_family      = AF_INET;
	stServer.Serveraddr.sin_addr.s_addr = inet_addr(szServerIP);
	stServer.Serveraddr.sin_port        = htons(atoi(szServerPort));
}

void UserObject::vGetHostID(HWND hName, HWND hPassWord)
{
	GetWindowText(hName,stHost.szUserName,32);
	GetWindowText(hPassWord,stHost.szUserPassWord,32);    
}

struct sockaddr_in UserObject::stGetTargetAddr()
{
	sockaddr_in addr;
    int iNumber;
	int iID;
    
	iNumber = 0;

	iID = 0;	
	
	switch(stTarget.iTargetType)
	{
	case TOUSER:
		{
            while(iNumber < MAX_USER && stUser[iNumber].iIndex != stTarget.iIndex)
			{
				iNumber++;
			}

			addr = stUser[iNumber].Useraddr;

			break;
		}
	case TOGROUP:
		{
			while(iNumber < MAX_GROUP && stGroup[iNumber].iIndex != stTarget.iIndex)
			{
				iNumber++;
			}

			iID = stGroup[iNumber].iOwnerID;

			iNumber = 0;

			while(iNumber < MAX_USER && stUser[iNumber].iUserID != iID)
			{
				iNumber++;
			}

			addr = stUser[iNumber].Useraddr;

		    break;
		}
	case TOSERVER:
		{
			addr = stServer.Serveraddr;
		    break;
		}
	  break;
	}
	return addr;
}

bool UserObject::bSetGroupTarget(HWND hGroupName)
{
    int i = 0;
	bool bFind = false;

	char szGroupName[INFO_LENGTH];

	GetWindowText(hGroupName,szGroupName,INFO_LENGTH);

	while(i < MAX_GROUP && !bFind)
	{
	  if ( strcmp(stGroup[i].szGroupName,szGroupName) == 0 && stGroup[i].iGroupID != EMPTYCONTENT)
	  {
		 stTarget.iIndex = i;

         bFind = true;
	  }

	  i++;
	}

	return bFind;
}

void UserObject::vInitSocketInfo()
{
    int iNumber;

	iNumber = 0;

	while(iNumber<MAX_SOCKET)
	{
         stSocketInfo[iNumber].iInfo = EMPTYCONTENT;

		 stSocketInfo[iNumber].iMessageLength = 0;

         stSocketInfo[iNumber].iReceived = 0;

         stSocketInfo[iNumber].szTempBuffer = NULL;
		 
		 iNumber++;
	}
}

void UserObject::vInitUserList()
{
	int iNumber;

    iNumber = 0;

	while (iNumber < MAX_USER)
	{
		memset(&stUser[iNumber].Useraddr,0,sizeof(sockaddr_in));

	    sprintf(stUser[iNumber].szUserName,"");

	    stUser[iNumber].iUserID = EMPTYCONTENT;

	    stUser[iNumber].iSocketNO = EMPTYCONTENT;

	    stUser[iNumber].iIndex = EMPTYCONTENT;

		iNumber++;
	}
}

void UserObject::vInitGroupList()
{
	int iNumberA;
	int iNumberB;

    iNumberA = 0;

	while (iNumberA < MAX_GROUP)
	{
		iNumberB = 0;

	    strcpy(stGroup[iNumberA].szGroupName,"");

		strcpy(stGroup[iNumberA].szGroupPassWord,"");

	    stGroup[iNumberA].iGroupID = EMPTYCONTENT;

	    stGroup[iNumberA].iOwnerID = EMPTYCONTENT;

	    stGroup[iNumberA].iIndex = EMPTYCONTENT;

		stGroup[iNumberA].bJoin = false;

		while (iNumberB < MAX_USER)
		{
			stGroup[iNumberA].iUserID[iNumberB] = EMPTYCONTENT;

			iNumberB++;
		}

		iNumberA++;
	}
}

void UserObject::vInitServer()
{
	memset(&stServer.Serveraddr,0,sizeof(sockaddr_in));

	stServer.iSocketNO = EMPTYCONTENT;

	stServer.iServerID = EMPTYCONTENT;
}

void UserObject::vInitHost()
{
	memset(&stHost.Useraddr,0,sizeof(sockaddr_in));

	strcpy(stHost.szUserName,"");

	strcpy(stHost.szUserPassWord,"");

	stHost.iUserID = EMPTYCONTENT;
}

void UserObject::vInitTarget()
{
	stTarget.iTargetType = EMPTYCONTENT;

	stTarget.iIndex = EMPTYCONTENT;	
}


int UserObject::iGetSocket()
{
	int n = 0;
	bool flag = false ;

    while ( flag == false && n < MAX_SOCKET)
	{
	  if (stSocketInfo[n].iInfo == EMPTYCONTENT)
	  {
		  return n;
	  }

      n++;
	}

	return n;
}

void UserObject::vCleanSocket(int iSocketNo)
{
	int iUserNo;

	iUserNo = 0;

	if (stSocketInfo[iSocketNo].iInfo != EMPTYCONTENT)
	{
		stSocketInfo[iSocketNo].iInfo = EMPTYCONTENT;
		
		closesocket(stSocketInfo[iSocketNo].socket);
	}
	
	if (stSocketInfo[iSocketNo].szTempBuffer != NULL)
	{
		delete [] stSocketInfo[iSocketNo].szTempBuffer;
		
		stSocketInfo[iSocketNo].szTempBuffer = NULL;
	}

	iUserNo = iFindUserFromSocketNo(iSocketNo);

	if( iUserNo != EMPTYCONTENT)
	{
		stUser[iUserNo].iSocketNO = EMPTYCONTENT;
	}
}

int UserObject::iFindUserFromSocketNo(int iSocketNo)
{
	int  iUserNo;
	bool bFlag;

	iUserNo = 0;

	bFlag = false;

	while(iUserNo<MAX_USER && !bFlag)
	{
		if(stUser[iUserNo].iSocketNO == iSocketNo)
		{
			bFlag = true;
		}
		else
		{
			iUserNo++;
		}
	}

	if (bFlag)
	{
		return iUserNo;
	}

	else
	{
		return EMPTYCONTENT;
	}
}

int UserObject::iFindUserFromAddr(struct sockaddr_in stSockaddr)
{
	int  iNumber;
	bool bFlag;

	iNumber = 0;

	bFlag = false;

	while(iNumber<MAX_USER && !bFlag)
	{
		if((strcmp(inet_ntoa(stUser[iNumber].Useraddr.sin_addr),inet_ntoa(stSockaddr.sin_addr)) == 0) && (stUser[iNumber].Useraddr.sin_port == stSockaddr.sin_port))
		{
			bFlag = true;
		}
		else
		{
			iNumber++;
		}
	}

	if(bFlag)
	{
		return iNumber;
	}

	else
	{
		return EMPTYCONTENT;
	}
}

int UserObject::iFindUserFromUserName(char * szUserName)
{
	int  iNumber;
	bool bFlag;

	iNumber = 0;

	bFlag = false;

	while(iNumber<MAX_USER && !bFlag)
	{
		if(strcmp(stUser[iNumber].szUserName,szUserName) == 0)
		{
			bFlag = true;
		}
		else
		{
			iNumber++;
		}
	}

	if(bFlag)
	{
		return iNumber;
	}

	else
	{
		return EMPTYCONTENT;
	}
}

int UserObject::iFindUserFromUserID(int iUserID)
{
	int  iNumber;
	bool bFlag;

	iNumber = 0;

	bFlag = false;

	while(iNumber<MAX_USER && !bFlag)
	{
		if(stUser[iNumber].iUserID == iUserID)
		{
			bFlag = true;
		}
		else
		{
			iNumber++;
		}
	}

	if(bFlag)
	{
		return iNumber;
	}

	else
	{
		return EMPTYCONTENT;
	}
}


int UserObject::iFindGroupFromGroupID(int iGroupID)
{
	int  iNumber;
	bool bFlag;

	iNumber = 0;

	bFlag = false;

	while(iNumber<MAX_GROUP && !bFlag)
	{
		if(stGroup[iNumber].iGroupID == iGroupID)
		{
			bFlag = true;
		}
		else
		{
			iNumber++;
		}
	}

	if(bFlag)
	{
		return iNumber;
	}

	else
	{
		return EMPTYCONTENT;
	}
}


MessageObject::MessageObject()
{
	vCleanMessageBuffer();
}

MessageObject::~MessageObject()
{
}

void MessageObject::vCleanMessageBuffer()
{
	memset(szMessageBuffer,'\0',sizeof(szMessageBuffer));
}

void MessageObject::vGetTransMessage()
{
	memcpy(&stTransMessage,szMessageBuffer,sizeof(szMessageBuffer));
}

void MessageObject::vSetTransMessage()
{
	memcpy(szMessageBuffer,&stTransMessage,sizeof(stTransMessage));
}
