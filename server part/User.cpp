#include "User.h"

// Constructor
UserObject::UserObject()
{
	vInitUserList();

	vInitGroupList();

	vInitSocketInfo();

	bServerOnLine = false;

	iMaxTextLength = 0;
}

// Destructor
UserObject::~UserObject()
{
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

    iNumberA = 0;

	while (iNumberA < MAX_GROUP)
	{
	    strcpy(stGroup[iNumberA].szGroupName,"");

		strcpy(stGroup[iNumberA].szGroupPassWord,"");

	    stGroup[iNumberA].iGroupID = EMPTYCONTENT;

	    stGroup[iNumberA].iOwnerID = EMPTYCONTENT;

	    stGroup[iNumberA].iIndex = EMPTYCONTENT;

		iNumberA++;
	}
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
	  else
	  {
		  n++;
	  }        
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

int UserObject::iFindUserFromAddr(char * szIPAddr)
{
	int  iNumber;
	bool bFlag;

	iNumber = 0;

	bFlag = false;

	while(iNumber<MAX_USER && !bFlag)
	{
		if(strcmp(inet_ntoa(stUser[iNumber].Useraddr.sin_addr),szIPAddr) == 0)
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
