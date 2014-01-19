#include <ctype.h>
#include "ChatProgramClient.h"
#include "User.h"
#include "error.h"

UserObject    UserInfo;
MessageObject MessageObject;
ErrorObject   ErrorObject;

char  *       lpLoopBuffer[MAX_USER];


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;
	WNDCLASSEX	wndclass;

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= fnMessageProcessor;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= lpszCLASSName;
	wndclass.hIconSm		= LoadIcon(NULL,IDI_APPLICATION);

	if( RegisterClassEx( &wndclass ) == 0 )
	{
		exit(1);
	}

	hMain_Window = CreateWindowEx(
		NULL,
		lpszCLASSName,
		lpszWINDOWSTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX,
		100,
		100,
		Windows_W,
		Windows_H,
		NULL,
		NULL,
		hInstance,
		NULL );

	vCreateInterface(hMain_Window,hInstance);

	EnableWindow(hButton_Send,FALSE);

	vLockGroupInput(true);

	ShowWindow(hMain_Window, nCmdShow);

	UpdateWindow(hMain_Window);

	if(!bInitProgram(hMain_Window))
	{
		vShowText(hOutput_Listbox, "<Program Init Fail>");

		exit(1);
	}

	vShowText(hOutput_Listbox, "<Program Init Successful>");

 	while( GetMessage ( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );

		DispatchMessage( &msg );
	}

	WSACleanup();

	return(msg.wParam);
}



LRESULT CALLBACK fnMessageProcessor ( HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	char		szMessage[256];
	int         iSocketNo;
	char        szBuffer[32];
	char        szName[32];
	int         iLine;
	int         iNumberA;
	char        szGroupName[INFO_LENGTH];
	char        szGroupPassword[INFO_LENGTH];
	int         iPort;
	int         iIndex;
	int         iUserID;
	int         iGroupID;
	int         iGroupNo;
	int         iGroup;
    int         iGroupOwner;
	int         iUserList[MAX_USER];
	bool        bLeft;
    char *      szToken;

	iSocketNo = 0;
	iNumberA  = 0;

	switch (iMsg)
    {
		case WM_COMMAND:
		{


			switch(LOWORD(wParam))
			{
			    case IDC_Input_GroupName:

					if (HIWORD(wParam) == EN_SETFOCUS)
					{
						SetWindowText(hInput_GroupName,"");

						SetWindowText(hButton_CreateGroup,"Create Group");
					}

					break;

				case IDC_Input_GroupPassword:

					if (HIWORD(wParam) == EN_SETFOCUS)
					{
						SetWindowText(hInput_GroupPassword,"");
					}

					break;

                case IDC_Output_FListbox:

					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						iLine = SendMessage(hOutput_FListbox,LB_GETCURSEL,0,0);

					    UserInfo.stTarget.iTargetType = TOUSER;

                        UserInfo.stTarget.iIndex = iLine;

						vShowUser();

                        SendMessage(hOutput_FListbox,LB_GETTEXT,iLine,(LPARAM)szBuffer);

				    	sprintf(szMessage,"Talk To %s",szBuffer);

                        SetWindowText(hText_TalkTo,szMessage);

						SendMessage(hOutput_GListbox,LB_SETCURSEL,-1,0);

					}

                    break;

				case IDC_Output_GListbox:

					iGroupNo = 0;

					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						iLine = SendMessage(hOutput_GListbox,LB_GETCURSEL,0,0);

					    UserInfo.stTarget.iTargetType = TOGROUP;

                        UserInfo.stTarget.iIndex = iLine;

					    SendMessage(hOutput_GListbox,LB_GETTEXT,iLine,(LPARAM)szBuffer);

					    SetWindowText(hInput_GroupName,&szBuffer[2]);

					    sprintf(szMessage,"Talk To Group %s",&szBuffer[2]);

                        SetWindowText(hText_TalkTo,szMessage);

						iGroupNo = iFindGroupFromListIndex(iLine);

						if(UserInfo.stGroup[iGroupNo].iOwnerID == UserInfo.stHost.iUserID)
						{
							SetWindowText(hButton_CreateGroup,"Destroy Group");
						}
						else
						{
							SetWindowText(hButton_CreateGroup,"Create Group");
						}

						if(UserInfo.stGroup[iGroupNo].bJoin == true)
						{
							SetWindowText(hButton_JoinGroup,"Leave Group");
						}
						else
						{
							SetWindowText(hButton_JoinGroup,"Join Group");
						}

						SendMessage(hOutput_FListbox,LB_SETCURSEL,-1,0);

						vShowGroupMember(UserInfo.stGroup[iGroupNo].iGroupID);
					}

                    break;

				case IDC_Button_Login:

					EnableWindow(hButton_Login,FALSE);

					if (UserInfo.bUserOnLine == false)
					{
						vLockServerInput(true);

					    EnableWindow(hButton_Register,FALSE);

                        UserInfo.vGetServerInfo(hInput_ServerIP,hInput_ServerPort);

					    iSocketNo = UserInfo.iGetSocket();

                   	    UserInfo.stSocketInfo[iSocketNo].socket = socket(AF_INET, SOCK_STREAM, 0);

					    if (WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket, hWnd, WM_SOCKET, FD_CONNECT | FD_CLOSE))
						{
						   vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

                           vLockServerInput(false);

						   EnableWindow(hButton_Login,TRUE);

					       EnableWindow(hButton_Register,TRUE);

						   closesocket(UserInfo.stSocketInfo[iSocketNo].socket);

						   break;
						}

					    UserInfo.stTarget.iTargetType = TOSERVER;

                        connect(UserInfo.stSocketInfo[iSocketNo].socket, (struct sockaddr*)&UserInfo.stGetTargetAddr(),sizeof(sockaddr));

						UserInfo.stSocketInfo[iSocketNo].iInfo = LOGIN_REQUEST;

					}

					else
					{

						if (UserInfo.stSocketInfo[UserInfo.stServer.iSocketNO].iInfo == IDLE)
						{
							UserInfo.stSocketInfo[UserInfo.stServer.iSocketNO].iInfo = LOGOUT_REQUEST;

							if(WSAAsyncSelect(UserInfo.stSocketInfo[UserInfo.stServer.iSocketNO].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
							{
								vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

	                            break;
							}
						}

						else
						{
							PostMessage(hWnd,WM_COMMAND,wParam,lParam);
						}
					}

					break;

				case IDC_Button_Register:

					memset(szBuffer,0,sizeof(szBuffer));

					GetWindowText(hInput_UserName,szBuffer,INFO_LENGTH);

					if(strlen(szBuffer) == 0)
					{
						vShowText(hOutput_Listbox,"<Require User Name>");

						break;
					}

					if(strlen(szBuffer) > 10)
					{
						vShowText(hOutput_Listbox,"<User Name Can Not Longer Than 10 Char>");

						break;
					}

					if(isspace(szBuffer[0]))
					{
						vShowText(hOutput_Listbox,"<First char of user name can not be space>");

						break;
					}

					memset(szBuffer,0,sizeof(szBuffer));

					GetWindowText(hInput_UserPassword,szBuffer,INFO_LENGTH);

					if(strlen(szBuffer) == 0)
					{
						vShowText(hOutput_Listbox,"<Require User Password>");

						break;
					}

					vLockServerInput(true);

					EnableWindow(hButton_Login,FALSE);

					EnableWindow(hButton_Register,FALSE);

                    UserInfo.vGetServerInfo(hInput_ServerIP,hInput_ServerPort);

					iSocketNo = UserInfo.iGetSocket();

					UserInfo.stSocketInfo[iSocketNo].socket = socket(AF_INET, SOCK_STREAM, 0);

					if (WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket, hWnd, WM_SOCKET, FD_CONNECT | FD_CLOSE))
					{
						vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

						vLockServerInput(false);

					    EnableWindow(hButton_Login,TRUE);

						EnableWindow(hButton_Register,TRUE);

						closesocket(UserInfo.stSocketInfo[iSocketNo].socket);

						break;
					}

                    UserInfo.stTarget.iTargetType = TOSERVER;

                    connect(UserInfo.stSocketInfo[iSocketNo].socket, (struct sockaddr*)&UserInfo.stGetTargetAddr(),sizeof(sockaddr));

					UserInfo.stSocketInfo[iSocketNo].iInfo = RESGISTER_REQUEST;

					break;

				case IDC_Button_Send:

					if (UserInfo.stTarget.iTargetType == TOSERVER)
					{
						vShowText(hOutput_Listbox, "<Please select a target first>");

						break;
					}

					memset(szMessage,0,sizeof(szMessage));

					GetWindowText(hInput_Editbox,szMessage,256);

					if (strlen(szMessage) == 0)
					{
						vShowText(hOutput_Listbox, "<Please Input Message first>");

						break;
					}

					if (strlen(szMessage) > 250)
					{
						vShowText(hOutput_Listbox, "<Message Can Not Longer Than 250 Char>");

						break;
					}

					EnableWindow(hButton_Send,FALSE);

					SendMessage(hInput_Editbox,EM_SETREADONLY,TRUE,NULL);


					if (UserInfo.stTarget.iTargetType == TOUSER)
					{
						if(bSendToUser(UserInfo.stTarget.iIndex,szMessage,EMPTYCONTENT))
						{
							EnableWindow(hButton_Send,TRUE);

							SetWindowText(hInput_Editbox,"");

							SendMessage(hInput_Editbox,EM_SETREADONLY,FALSE,NULL);

							memset(szName,0,sizeof(szName));

							memset(szBuffer,0,sizeof(szBuffer));

					        GetWindowText(hText_TalkTo,szName,32);

							sprintf(szBuffer,"<%s>",szName);

							vShowText(hOutput_Listbox, szBuffer);

							szToken = strtok(szMessage,"\r\n");

		                    while (szToken!= NULL)
							{
								vShowText(hOutput_Listbox,szToken);

			                    szToken = strtok(NULL,"\r\n");
							}
						}


						else
						{
							PostMessage(hWnd,WM_COMMAND,wParam,lParam);
						}
						break;
					}

					if (UserInfo.stTarget.iTargetType == TOGROUP)
					{
						iNumberA = 0;

						while(iNumberA < MAX_GROUP && UserInfo.stGroup[iNumberA].iIndex != UserInfo.stTarget.iIndex)
						{
							iNumberA++;
						}

						if (!UserInfo.stGroup[iNumberA].bJoin)
						{
							vShowText(hOutput_Listbox, "<Please Join Group First>");

							EnableWindow(hButton_Send,TRUE);

							SetWindowText(hInput_Editbox,"");

							SendMessage(hInput_Editbox,EM_SETREADONLY,FALSE,NULL);

						    break;
						}


						iNumberA = 0;

		                while(iNumberA < MAX_USER)
						{
							iUserList[iNumberA] = EMPTYCONTENT;

                            iNumberA++;
						}

						iNumberA = 0;

						iIndex = 0;

						while(iNumberA < MAX_GROUP && UserInfo.stGroup[iNumberA].iIndex != UserInfo.stTarget.iIndex)
						{
							iNumberA++;
						}

						iIndex = iNumberA;

						iNumberA = 0;

		                while(iNumberA < MAX_USER)
						{
							if(UserInfo.stGroup[iIndex].iUserID[iNumberA] != EMPTYCONTENT && UserInfo.stGroup[iIndex].iUserID[iNumberA] != UserInfo.stHost.iUserID)
							{
								iUserList[iNumberA] = UserInfo.stGroup[iIndex].iUserID[iNumberA];
							}

			                iNumberA++;
						}

						iNumberA = 0;

		                while(lpLoopBuffer[iNumberA] != NULL)
						{
							iNumberA++;
						}

						lpLoopBuffer[iNumberA] = new char[1024];

						memcpy(lpLoopBuffer[iNumberA],&UserInfo.stGroup[iIndex].iGroupID,sizeof(int));

		                memcpy(lpLoopBuffer[iNumberA] + sizeof(int),szMessage,sizeof(szMessage));

		                memcpy(lpLoopBuffer[iNumberA] + sizeof(int) + sizeof(szMessage),iUserList,sizeof(iUserList));

                        PostMessage(hMain_Window,WM_LOOP,MESSAGE,(LPARAM)lpLoopBuffer[iNumberA]);

						EnableWindow(hButton_Send,TRUE);

						SetWindowText(hInput_Editbox,"");

						SendMessage(hInput_Editbox,EM_SETREADONLY,FALSE,NULL);

						memset(szName,0,sizeof(szName));

						memset(szBuffer,0,sizeof(szBuffer));

					    GetWindowText(hText_TalkTo,szName,32);

						sprintf(szBuffer,"<%s>",szName);

						vShowText(hOutput_Listbox, szBuffer);

						szToken = strtok(szMessage,"\r\n");

		                while (szToken!= NULL)
						{
							vShowText(hOutput_Listbox,szToken);

			                szToken = strtok(NULL,"\r\n");
						}

						break;
					}

					break;

				case IDC_Button_CreateGroup:

					memset(szMessage,0,sizeof(szMessage));

					GetWindowText(hInput_GroupName,szMessage,16);

					if(strlen(szMessage) == 0)
					{
						vShowText(hOutput_Listbox,"<Group Name Can Not Be Empty>");

						break;
					}

					if(strlen(szMessage) > 10)
					{
						vShowText(hOutput_Listbox,"<Group Name Can Not Longer Than 10 Char>");

						break;
					}

					memset(szMessage,0,sizeof(szMessage));

					vLockGroupInput(true);

                    if (UserInfo.stSocketInfo[UserInfo.stServer.iSocketNO].iInfo == IDLE)
					{
						GetWindowText(hButton_CreateGroup,szMessage,16);

						if(strcmp(szMessage,"Create Group") == 0)
						{
							UserInfo.stSocketInfo[UserInfo.stServer.iSocketNO].iInfo = CREATEGROUP_REQUEST;
						}

						else
						{
							UserInfo.stSocketInfo[UserInfo.stServer.iSocketNO].iInfo = DESTROYGROUP_REQUEST;
						}

                    if(WSAAsyncSelect(UserInfo.stSocketInfo[UserInfo.stServer.iSocketNO].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
					{
						vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

	                    break;
					}

						break;
					}

					else
					{
						PostMessage(hWnd,WM_COMMAND,wParam,lParam);

						break;
					}

					break;

				case IDC_Button_JoinGroup:

					iGroup = 0;

					iGroupOwner = 0;

					vLockGroupInput(true);

					if (!UserInfo.bSetGroupTarget(hInput_GroupName))
					{
					   vShowText(hOutput_Listbox, "<Unkown Group>");

					   vLockGroupInput(false);

					   break;
					}

					iGroup = iFindGroupFromListIndex(UserInfo.stTarget.iIndex);

					if (UserInfo.stGroup[iGroup].iOwnerID == UserInfo.stHost.iUserID)
					{
					   vShowText(hOutput_Listbox, "<You can't join or left a group as a owner>");

					   vLockGroupInput(false);

					   break;
					}

					iGroupOwner = UserInfo.iFindUserFromUserID(UserInfo.stGroup[iGroup].iOwnerID);

					if(UserInfo.stUser[iGroupOwner].iSocketNO == EMPTYCONTENT)
					{
						iSocketNo = UserInfo.iGetSocket();

                        UserInfo.stSocketInfo[iSocketNo].socket = socket(AF_INET, SOCK_STREAM, 0);

                        if (WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket, hWnd, WM_SOCKET, FD_CONNECT | FD_CLOSE))
						{
							vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

							vLockGroupInput(false);

							break;
						}

						UserInfo.stTarget.iTargetType = TOGROUP;

						connect(UserInfo.stSocketInfo[iSocketNo].socket, (struct sockaddr*)&UserInfo.stGetTargetAddr(),sizeof(sockaddr));

						if (UserInfo.stGroup[iGroup].bJoin)
						{
							UserInfo.stSocketInfo[iSocketNo].iInfo = LEFTGROUP_REQUEST;
						}

						else
						{
							UserInfo.stSocketInfo[iSocketNo].iInfo = JOINGROUP_REQUEST;
						}

						break;
					}

					else
					{
						if (UserInfo.stSocketInfo[UserInfo.stUser[iGroupOwner].iSocketNO].iInfo == IDLE)
						{
							if (UserInfo.stGroup[iGroup].bJoin)
							{
								UserInfo.stSocketInfo[UserInfo.stUser[iGroupOwner].iSocketNO].iInfo = LEFTGROUP_REQUEST;
							}

							else
							{
								UserInfo.stSocketInfo[UserInfo.stUser[iGroupOwner].iSocketNO].iInfo = JOINGROUP_REQUEST;
							}

							if(WSAAsyncSelect(UserInfo.stSocketInfo[UserInfo.stUser[iGroupOwner].iSocketNO].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
							{
								vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

								break;
							}

						    break;
						}

					    else
						{
							PostMessage(hWnd,WM_COMMAND,wParam,lParam);

						    break;
						}

					}

					break;
			}

			break;
		}

		case WM_DESTROY:

            iNumberA = 0;

			while (iNumberA < MAX_SOCKET)
			{
				UserInfo.vCleanSocket(iNumberA);

			    iNumberA++;
			}

			iNumberA = 0;

			while(iNumberA < MAX_USER)
			{
				if (lpLoopBuffer[iNumberA] != NULL)
				{
					delete [] lpLoopBuffer[iNumberA];

					lpLoopBuffer[iNumberA] = NULL;
				}

				iNumberA++;
			}

			WSACleanup();

			PostQuitMessage(0);

			break;

        case WM_NCHITTEST:
			{
				LRESULT   uHit   =   DefWindowProc(hWnd,   WM_NCHITTEST,   wParam,   lParam);

                switch(uHit)
				{
				   case HTLEFT:
                   case HTRIGHT:
                   case HTTOP:
                   case HTBOTTOM:
                   case HTTOPLEFT:
                   case HTTOPRIGHT:
                   case HTBOTTOMRIGHT:
                   case HTBOTTOMLEFT:
                   return HTBORDER;
				}

				return   uHit;
			}

			break;

        case WM_LOOP:


			iUserID = 0;

			iGroupID = 0;

			iNumberA = 0;

			iIndex = 0;

			iSocketNo = 0;

			bLeft = false;

			iNumberA = 0;

		    while(iNumberA < MAX_USER)
			{
			   iUserList[iNumberA] = EMPTYCONTENT;

               iNumberA++;
			}

			iNumberA = 0;

			switch (wParam)
			{
			   case ADDGROUPMEMBER:

			       memcpy(&iUserID,(char *)lParam,sizeof(int));

			       memcpy(&iGroupID,(char *)lParam + sizeof(int),sizeof(int));

			       memcpy(iUserList,(char *)lParam + sizeof(int)*2,sizeof(iUserList));

				   while(iNumberA < MAX_USER)
				   {
					   if(iUserList[iNumberA] != EMPTYCONTENT && iUserList[iNumberA] != UserInfo.stHost.iUserID)
					   {
						   iIndex = UserInfo.iFindUserFromUserID(iUserList[iNumberA]);

					       iSocketNo = UserInfo.stUser[iIndex].iSocketNO;

					       if(UserInfo.stSocketInfo[iSocketNo].iInfo == IDLE)
						   {
							   UserInfo.stSocketInfo[iSocketNo].iInfo = ADDGROUPMEMBER;

							   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
							   {
								   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				                   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
							   }

							   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[sizeof(int)*2];

							   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iUserID,sizeof(int));

							   memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)],&iGroupID,sizeof(int));

							   if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
							   {
								   vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

								   break;
							   }

                               iUserList[iNumberA] = EMPTYCONTENT;
						   }
						   else
						   {
							   bLeft = true;
						   }
					   }

					   iNumberA++;
				   }

				   if(bLeft)
				   {
					   memcpy((char *)lParam,&iUserID,sizeof(int));

					   memcpy((char *)lParam + sizeof(int),&iGroupID,sizeof(int));

				       memcpy((char *)lParam + sizeof(int)*2,iUserList,sizeof(iUserList));

					   PostMessage(hWnd,WM_LOOP,ADDGROUPMEMBER,lParam);
				   }

				   else
				   {
					   delete [] (char *)lParam;

					   iNumberA = 0;

					   while(iNumberA <MAX_USER)
					   {
						   if(lpLoopBuffer[iNumberA] == (char *)lParam)
						   {
							   lpLoopBuffer[iNumberA] = NULL;
						   }

						   iNumberA++;
					   }
				   }

				   break;

			   case REMOVEGROUPMEMBER:

			       memcpy(&iUserID,(char *)lParam,sizeof(int));

			       memcpy(&iGroupID,(char *)lParam + sizeof(int),sizeof(int));

			       memcpy(iUserList,(char *)lParam + sizeof(int)*2,sizeof(iUserList));

				   while(iNumberA < MAX_USER)
				   {
					   if(iUserList[iNumberA] != EMPTYCONTENT)
					   {
						   iIndex = UserInfo.iFindUserFromUserID(iUserList[iNumberA]);

					       iSocketNo = UserInfo.stUser[iIndex].iSocketNO;

					       if(UserInfo.stSocketInfo[iSocketNo].iInfo == IDLE)
						   {
							   UserInfo.stSocketInfo[iSocketNo].iInfo = REMOVEGROUPMEMBER;

							   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
							   {
								   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				                   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
							   }

							   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[sizeof(int)*2];

							   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iUserID,sizeof(int));

							   memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)],&iGroupID,sizeof(int));

							   if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
							   {
								   vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

								   break;
							   }

                               iUserList[iNumberA] = EMPTYCONTENT;
						   }
						   else
						   {
							   bLeft = true;
						   }
					   }

					   iNumberA++;
				   }

				   if(bLeft)
				   {
					   memcpy((int *)lParam,&iUserID,sizeof(int));

					   memcpy((int *)lParam + 1,&iGroupID,sizeof(int));

				       memcpy((int *)lParam + 2,iUserList,sizeof(iUserList));
				   }

				   else
				   {
					   delete [] (char *)lParam;

					   iNumberA = 0;

					   while(iNumberA <MAX_USER)
					   {
						   if(lpLoopBuffer[iNumberA] == (char *)lParam)
						   {
							   lpLoopBuffer[iNumberA] = NULL;
						   }

						   iNumberA++;
					   }
				   }

				   break;

			   case MESSAGE:

                   iGroupID = 0;

                   memset(szMessage,0,sizeof(szMessage));

				   memcpy(&iGroupID,(char *)lParam,sizeof(int));

			       memcpy(szMessage,(char *)lParam + sizeof(int),sizeof(szMessage));

			       memcpy(iUserList,(char *)lParam + sizeof(int) + sizeof(szMessage),sizeof(iUserList));

				   while(iNumberA < MAX_USER)
				   {
					   if(iUserList[iNumberA] != EMPTYCONTENT)
					   {
						   iIndex = UserInfo.iFindUserFromUserID(iUserList[iNumberA]);

					       if(bSendToUser(iIndex,szMessage,iGroupID))
						   {
							   iUserList[iNumberA] = EMPTYCONTENT;
						   }

						   else
						   {
							   bLeft = true;
						   }
					   }

					   iNumberA++;
				   }

				   if(bLeft)
				   {
					   memcpy((char *)lParam,&iGroupID,sizeof(int));

					   memcpy((char *)lParam + sizeof(int),szMessage,sizeof(szMessage));

				       memcpy((char *)lParam + sizeof(int) + sizeof(szMessage),iUserList,sizeof(iUserList));
				   }

				   else
				   {
					   delete [] (char *)lParam;

					   iNumberA = 0;

					   while(iNumberA <MAX_USER)
					   {
						   if(lpLoopBuffer[iNumberA] == (char *)lParam)
						   {
							   lpLoopBuffer[iNumberA] = NULL;
						   }

						   iNumberA++;
					   }
				   }


				   break;

			}

            break;

		case WM_SOCKET:

			  struct sockaddr_in stAddr;
			  int    iPeerAddrLength;

			  iSocketNo = 0;

			  while (wParam != UserInfo.stSocketInfo[iSocketNo].socket)
			  {
				  iSocketNo++;
			  }

			  if (WSAGETSELECTERROR(lParam))
			  {
				  ErrorObject.vGetWSAErrorMessage(WSAGETSELECTERROR(lParam));

				  vShowSocketError(iSocketNo);

				  if (UserInfo.stServer.iSocketNO == iSocketNo || UserInfo.stServer.iSocketNO == EMPTYCONTENT)
				  {
					  vUserLogout();
				  }

				  else
				  {
					  UserInfo.vCleanSocket(iSocketNo);
				  }

				  break;
			  }

			switch (WSAGETSELECTEVENT(lParam))
			{

			  case FD_CONNECT:

				  memset(&stAddr,0,sizeof(sockaddr_in));

			      iPeerAddrLength = sizeof(struct sockaddr_in);

				  getpeername(wParam,(struct sockaddr*)&stAddr, &iPeerAddrLength);

				  if((strcmp(inet_ntoa(stAddr.sin_addr),inet_ntoa(UserInfo.stServer.Serveraddr.sin_addr)) == 0) && (stAddr.sin_port == UserInfo.stServer.Serveraddr.sin_port))
				  {
					  UserInfo.stServer.iSocketNO = iSocketNo;
				  }

				  else
				  {
					  iNumberA = UserInfo.iFindUserFromAddr(stAddr);

					  if (iNumberA != EMPTYCONTENT)
					  {
						  UserInfo.stUser[iNumberA].iSocketNO = iSocketNo;
					  }
				  }

				  WSAAsyncSelect(wParam,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);

				  break;


			  case FD_ACCEPT:

				  iSocketNo = UserInfo.iGetSocket();

				  UserInfo.stSocketInfo[iSocketNo].socket = socket(AF_INET, SOCK_STREAM, 0);

				  UserInfo.stSocketInfo[iSocketNo].socket = accept(wParam,NULL,NULL);

				  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
				  {
					  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

					  closesocket(UserInfo.stSocketInfo[iSocketNo].socket);

					  break;
				  }

                  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

				  break;

			  case FD_READ:

				  UserInfo.stSocketInfo[iSocketNo].iInfo = READING;

				  if (bGetPacket(iSocketNo))
				  {
					  vProcessReadMessage(iSocketNo);

					  UserInfo.stSocketInfo[iSocketNo].iMessageLength = 0 ;

                      if(UserInfo.stSocketInfo[iSocketNo].iInfo != EMPTYCONTENT)
					  {
						  if (WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

						      break;
						  }
					  }
				  }

				  break;

			  case FD_WRITE:

				  switch (UserInfo.stSocketInfo[iSocketNo].iInfo)
				  {
				    case LOGIN_REQUEST:

						iPort = 0;

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = LOGIN_REQUEST;

						iPort = ntohs(UserInfo.stHost.Useraddr.sin_port);

						memcpy(MessageObject.stTransMessage.szAddMessage,&iPort,sizeof(int));

						GetWindowText(hInput_UserName,UserInfo.stHost.szUserName,INFO_LENGTH);

						GetWindowText(hInput_UserPassword,UserInfo.stHost.szUserPassWord,INFO_LENGTH);

						memcpy(&MessageObject.stTransMessage.szAddMessage[sizeof(int)],UserInfo.stHost.szUserName,sizeof(UserInfo.stHost.szUserName));

						memcpy(&MessageObject.stTransMessage.szAddMessage[sizeof(int)+sizeof(UserInfo.stHost.szUserName)],UserInfo.stHost.szUserPassWord,sizeof(UserInfo.stHost.szUserPassWord));

                        MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender) + sizeof(UserInfo.stHost.szUserName) + sizeof(UserInfo.stHost.szUserPassWord);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

					    break;

				    case LOGOUT_REQUEST:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = LOGOUT_REQUEST;

						memcpy(MessageObject.stTransMessage.szAddMessage,&UserInfo.stHost.iUserID,sizeof(UserInfo.stHost.iUserID));

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender) + sizeof(UserInfo.stHost.iUserID);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						break;

				    case RESGISTER_REQUEST:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = RESGISTER_REQUEST;

						GetWindowText(hInput_UserName,UserInfo.stHost.szUserName,INFO_LENGTH);

						GetWindowText(hInput_UserPassword,UserInfo.stHost.szUserPassWord,INFO_LENGTH);

						memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stHost.szUserName,sizeof(UserInfo.stHost.szUserName));

						memcpy(&MessageObject.stTransMessage.szAddMessage[sizeof(UserInfo.stHost.szUserName)],UserInfo.stHost.szUserPassWord,sizeof(UserInfo.stHost.szUserPassWord));

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender) + sizeof(UserInfo.stHost.szUserName) + sizeof(UserInfo.stHost.szUserPassWord);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						break;

				    case CREATEGROUP_REQUEST:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						memset(szGroupName,0,sizeof(szGroupName));

						memset(szGroupPassword,0,sizeof(szGroupPassword));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = CREATEGROUP_REQUEST;

						GetWindowText(hInput_GroupName,szGroupName,INFO_LENGTH);

						GetWindowText(hInput_GroupPassword,szGroupPassword,INFO_LENGTH);

						memcpy(MessageObject.stTransMessage.szAddMessage,&UserInfo.stHost.iUserID,sizeof(int));

						memcpy(&MessageObject.stTransMessage.szAddMessage[sizeof(int)],szGroupName,sizeof(szGroupName));

						memcpy(&MessageObject.stTransMessage.szAddMessage[sizeof(int)+sizeof(szGroupName)],szGroupPassword,sizeof(szGroupPassword));

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender) + sizeof(szGroupName) + sizeof(szGroupPassword);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						break;

					case DESTROYGROUP_REQUEST:

						iNumberA = 0;

						while(UserInfo.stGroup[iNumberA].iIndex != UserInfo.stTarget.iIndex)
						{
							iNumberA++;
						}

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = DESTROYGROUP_REQUEST;

						memcpy(MessageObject.stTransMessage.szAddMessage,&UserInfo.stGroup[iNumberA].iGroupID,sizeof(int));

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						break;

					case JOINGROUP_REQUEST:

						iNumberA = 0;

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						memset(szGroupPassword,0,sizeof(szGroupPassword));

						while(UserInfo.stGroup[iNumberA].iIndex != UserInfo.stTarget.iIndex)
						{
							iNumberA++;
						}

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = JOINGROUP_REQUEST;

						memcpy(MessageObject.stTransMessage.szAddMessage,&UserInfo.stGroup[iNumberA].iGroupID,sizeof(int));

						GetWindowText(hInput_GroupPassword,szGroupPassword,INFO_LENGTH);

						memcpy(&MessageObject.stTransMessage.szAddMessage[sizeof(int)],szGroupPassword,sizeof(szGroupPassword));

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender) + sizeof(szGroupPassword);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						break;

					case JOINGROUP_SUCCESS:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer == NULL)
						{
							UserInfo.stSocketInfo[iSocketNo].iInfo = JOINGROUP_FAIL;

							break;
						}

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = JOINGROUP_SUCCESS;

						memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,(sizeof(int)*(MAX_USER+1)));

						delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

						UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*(MAX_USER+3) + sizeof(MessageObject.stTransMessage.szSender);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

						break;

					case JOINGROUP_FAIL:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = JOINGROUP_FAIL;

						if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						{
							memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,strlen(UserInfo.stSocketInfo[iSocketNo].szTempBuffer));

							delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						}

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender) + strlen(MessageObject.stTransMessage.szAddMessage) + 1;

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

						break;

					case ADDGROUPMEMBER:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = ADDGROUPMEMBER;

						if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer == NULL)
						{
							break;
						}

						memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int));

						memcpy(&MessageObject.stTransMessage.szAddMessage[sizeof(int)],&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)],sizeof(int));

                        delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

						UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*4 + sizeof(MessageObject.stTransMessage.szSender);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

						break;

					case LEFTGROUP_REQUEST:

						iNumberA = 0;

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						while(UserInfo.stGroup[iNumberA].iIndex != UserInfo.stTarget.iIndex)
						{
							iNumberA++;
						}

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = LEFTGROUP_REQUEST;

						memcpy(MessageObject.stTransMessage.szAddMessage,&UserInfo.stGroup[iNumberA].iGroupID,sizeof(int));

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						break;

					case LEFTGROUP_SUCCESS:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						{
							MessageObject.stTransMessage.iMessageType = LEFTGROUP_SUCCESS;

							memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int));

							delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						}

						else
						{
							MessageObject.stTransMessage.iMessageType = LEFTGROUP_FAIL;
						}

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

						break;

					case REMOVEGROUPMEMBER:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = REMOVEGROUPMEMBER;

						if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer == NULL)
						{
							break;
						}

						memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int));

						memcpy(&MessageObject.stTransMessage.szAddMessage[sizeof(int)],&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)],sizeof(int));

                        delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

						UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*4 + sizeof(MessageObject.stTransMessage.szSender);

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

						break;

					case MESSAGE:

						memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						sprintf(MessageObject.stTransMessage.szSender,UserInfo.stHost.szUserName);

						MessageObject.stTransMessage.iMessageType = MESSAGE;

						if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						{
							memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer, strlen(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)]) + sizeof(int) + 1);

							delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

						    UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						}

						MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender) + strlen(&MessageObject.stTransMessage.szAddMessage[sizeof(int)]) + sizeof(int) + 1;

						MessageObject.vSetTransMessage();

						send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

					    break;
                  }

				  break;

			  case FD_CLOSE:

				  if (UserInfo.stServer.iSocketNO == iSocketNo)
				  {
					  vShowText(hOutput_Listbox, "<Server Offline>");

					  vUserLogout();
				  }

				  else
				  {
					  UserInfo.vCleanSocket(iSocketNo);
				  }

				  break;

			}

			break;



		default:
			return(DefWindowProc(hWnd, iMsg, wParam, lParam));
	}
	return(0);
}



void vCreateInterface(HWND hWnd,HINSTANCE hInstance)
{
	hText_ServerIP = CreateWindow(
		"static","Server IP",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		5,
		120,
		20,
		hWnd,(HMENU)IDC_Text_ServerIP,hInstance,NULL);

	hText_ServerPort = CreateWindow(
		"static","Server Port",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		55,
		120,
		20,
		hWnd,(HMENU)IDC_Text_ServerPort,hInstance,NULL);

	hText_UserName = CreateWindow(
		"static","User Name",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		105,
		120,
		20,
		hWnd,(HMENU)IDC_Text_UserName,hInstance,NULL);

	hText_UserPassword = CreateWindow(
		"static","Password",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		155,
		120,
		20,
		hWnd,(HMENU)IDC_Text_UserPassword,hInstance,NULL);

	hText_GroupName = CreateWindow(
		"static","GroupName",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		260,
		120,
		20,
		hWnd,(HMENU)IDC_Text_GroupName,hInstance,NULL);

	hText_GroupPassword = CreateWindow(
		"static","GroupPassword",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		310,
		120,
		20,
		hWnd,(HMENU)IDC_Text_GroupPassword,hInstance,NULL);



	hInput_UserName = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT","Guest",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		Windows_W - 135,
		130,
		120,
		20,
		hWnd,(HMENU)IDC_Input_UserName,hInstance,NULL);

	hInput_ServerIP = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT","127.0.0.1",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT ,
		Windows_W - 135,
		30,
		120,
		20,
		hWnd,(HMENU)IDC_Input_ServerIP,hInstance,NULL);

	hInput_ServerPort = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT","8000",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		Windows_W - 135,
		80,
		120,
		20,
		hWnd,(HMENU)IDC_Input_ServerPort,hInstance,NULL);

	hInput_UserPassword = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT","",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		Windows_W - 135,
		180,
		120,
		20,
		hWnd,(HMENU)IDC_Input_UserPassword,hInstance,NULL);

	hInput_GroupName = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT","",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		Windows_W - 135,
		285,
		120,
		20,
		hWnd,(HMENU)IDC_Input_GroupName,hInstance,NULL);

	hInput_GroupPassword = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT","",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		Windows_W - 135,
		335,
		120,
		20,
		hWnd,(HMENU)IDC_Input_GroupPassword,hInstance,NULL);



	hButton_Login = CreateWindow(
		"BUTTON",
		"Login",
		WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON ,
		Windows_W - 135,
		205,
		120,
		20,
		hWnd,(HMENU)IDC_Button_Login,hInstance,NULL);

	hButton_Register = CreateWindow(
		"BUTTON",
		"Register",
		WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON ,
		Windows_W - 135,
		230,
		120,
		20,
		hWnd,(HMENU)IDC_Button_Register,hInstance,NULL);

    hButton_Send = CreateWindow(
		"BUTTON",
		"Send",
		WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON ,
		Windows_W - 135,
		420,
		120,
		20,
		hWnd,(HMENU)IDC_Button_Send,hInstance,NULL);

	hButton_CreateGroup = CreateWindow(
		"BUTTON",
		"Create Group",
		WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON ,
		Windows_W - 135,
		360,
		120,
		20,
		hWnd,(HMENU)IDC_Button_CreateGroup,hInstance,NULL);

	hButton_JoinGroup = CreateWindow(
		"BUTTON",
		"Join Group",
		WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON ,
		Windows_W - 135,
		385,
		120,
		20,
		hWnd,(HMENU)IDC_Button_JoinGroup,hInstance,NULL);



	hOutput_Listbox = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"LISTBOX",
		NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | WS_HSCROLL,
		Windows_W - 505,
		5,
		365,
		300,
		hWnd,(HMENU)IDC_Output_Listbox,hInstance,NULL);

	hInput_Editbox = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT",NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL,
		Windows_W - 505,
		330,
		365,
		110,
		hWnd,(HMENU)IDC_Input_Editbox,hInstance,NULL);

	hText_TalkTo = CreateWindow(
		"static","Talk To",
		WS_CHILD | SS_LEFT | WS_VISIBLE,
		Windows_W - 505,
		305,
		365,
		20,
		hWnd,(HMENU)IDC_Text_TalkTo,hInstance,NULL);



	hOutput_FListbox = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"LISTBOX",
		NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
		5,
		5,
		125,
		250,
		hWnd,(HMENU)IDC_Output_FListbox,hInstance,NULL);

	hOutput_GListbox = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"LISTBOX",
		NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
		5,
		255,
		125,
		195,
		hWnd,(HMENU)IDC_Output_GListbox,hInstance,NULL);
}


bool bInitProgram(HWND hWnd)
{
	int     iFlag;
	WSADATA wsaData;
	WORD	wVersionRequested;
	struct  sockaddr_in stAddr;
	int     iSocketNo;
	int     iHostPort;

	iSocketNo = 0;

	iHostPort = 6000;

	wVersionRequested = MAKEWORD(2, 2);

	iFlag = WSAStartup(wVersionRequested,&wsaData);

	if (iFlag != 0)
	{
       return false;
	}

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion ) != 2 )
	{
       WSACleanup();

       return false;
	}

	iSocketNo = UserInfo.iGetSocket();

    UserInfo.stSocketInfo[iSocketNo].socket = socket(AF_INET, SOCK_STREAM, 0);

    stAddr.sin_family      = AF_INET;

	stAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	stAddr.sin_port        = htons(iHostPort);

	while (iHostPort < 10000 && iFlag == 0)
	{
		if (bind(UserInfo.stSocketInfo[iSocketNo].socket,(struct sockaddr*)&stAddr,sizeof(struct  sockaddr_in)) == 0)
		{
			iFlag = 1;
		}
		else
		{
			iHostPort++;

		    stAddr.sin_port = htons(iHostPort);
		}
	}

	if (iFlag != 1)
	{
		return false;
	}

	if (WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE))
	{
		vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

	    return false;
	}

	listen(UserInfo.stSocketInfo[iSocketNo].socket,5);

	UserInfo.stHost.Useraddr.sin_port = stAddr.sin_port;

    UserInfo.stSocketInfo[iSocketNo].iInfo = LISTENSOCKET;

    return true;
}


void vShowText(HWND	hWnd, char *szText)
{
	int Line;
	int length;
	HDC  hdc;
    SIZE s;

	SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)szText);

	Line = SendMessage(hWnd,LB_GETCOUNT,0,0);

	SendMessage(hWnd,LB_SETCURSEL,Line-1,0);

	SendMessage(hWnd,LB_SETCURSEL,-1,0);

	length = strlen(szText);

	if(length > UserInfo.iMaxTextLength)
	{
		hdc = GetDC(hWnd);

		GetTextExtentPoint32(hdc, szText, strlen(szText), &s);

	    SendMessage(hWnd,LB_SETHORIZONTALEXTENT,s.cx+5,0);

        UserInfo.iMaxTextLength = length;
	}
}

void vLockServerInput(bool flag)
{
	if (flag)
	{
		SendMessage(hInput_ServerIP,EM_SETREADONLY,TRUE,NULL);

		SendMessage(hInput_ServerPort,EM_SETREADONLY,TRUE,NULL);

		SendMessage(hInput_UserName,EM_SETREADONLY,TRUE,NULL);

		SendMessage(hInput_UserPassword,EM_SETREADONLY,TRUE,NULL);
	}
	else
	{
		SendMessage(hInput_ServerIP,EM_SETREADONLY,FALSE,NULL);

		SendMessage(hInput_ServerPort,EM_SETREADONLY,FALSE,NULL);

		SendMessage(hInput_UserName,EM_SETREADONLY,FALSE,NULL);

		SendMessage(hInput_UserPassword,EM_SETREADONLY,FALSE,NULL);
	}
}

void vLockGroupInput(bool flag)
{
	if (flag)
	{
		EnableWindow(hButton_CreateGroup,FALSE);

		EnableWindow(hButton_JoinGroup,FALSE);

		SendMessage(hInput_GroupName,EM_SETREADONLY,TRUE,NULL);

		SendMessage(hInput_GroupPassword,EM_SETREADONLY,TRUE,NULL);
	}
	else
	{
		EnableWindow(hButton_CreateGroup,TRUE);

		EnableWindow(hButton_JoinGroup,TRUE);

		SendMessage(hInput_GroupName,EM_SETREADONLY,FALSE,NULL);

		SendMessage(hInput_GroupPassword,EM_SETREADONLY,FALSE,NULL);
	}
}


bool bSendToUser(int iIndex, char * szSendMessage, int iGroupID)
{
	int iUserNO;
	int iSocketNo;

    iUserNO   = 0;

	iSocketNo = 0;

	while ( iUserNO < MAX_USER && UserInfo.stUser[iUserNO].iIndex != iIndex)
	{
		iUserNO++;
	}

    if (UserInfo.stUser[iUserNO].iSocketNO == EMPTYCONTENT)
	{
		iSocketNo = UserInfo.iGetSocket();

		UserInfo.stSocketInfo[iSocketNo].socket = socket(AF_INET, SOCK_STREAM, 0);

		if (WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket, hMain_Window, WM_SOCKET, FD_CONNECT | FD_CLOSE))
		{
			vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

			return false;
		}

		connect(UserInfo.stSocketInfo[iSocketNo].socket, (struct sockaddr*)&UserInfo.stUser[iUserNO].Useraddr,sizeof(sockaddr));

		UserInfo.stSocketInfo[iSocketNo].iInfo = MESSAGE;

		if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
		{
			delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

			UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
		}

			UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [256];

			memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iGroupID,sizeof(int));

			memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)],szSendMessage,250);
	}
	else
	{
		if (UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].iInfo == IDLE)
		{
			UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].iInfo = MESSAGE;

			if (UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].szTempBuffer != NULL)
			{
				delete [] UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].szTempBuffer;

				UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].szTempBuffer = NULL;
			}

			UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].szTempBuffer = new char [256];

			memcpy(UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].szTempBuffer,&iGroupID,sizeof(int));

			memcpy(&UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].szTempBuffer[sizeof(int)],szSendMessage,250);

			if (WSAAsyncSelect(UserInfo.stSocketInfo[UserInfo.stUser[iUserNO].iSocketNO].socket,hMain_Window,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
			{
				vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
			}
		}
		else
		{
			return false;
		}

	}

	return true;
}

int iFindGroupFromListIndex(int iIndex)
{
	int iNumber;

	iNumber = 0;

	while(iNumber < MAX_GROUP && UserInfo.stGroup[iNumber].iIndex != iIndex)
	{
		iNumber++;
	}

	return iNumber;
}

void vShowSocketError(int iSocketNo)
{
	char szErrno[128];

	memset(szErrno,0,128);

	switch(UserInfo.stSocketInfo[iSocketNo].iInfo)
	{
		 case LOGIN_REQUEST:

			 sprintf(szErrno, "<Login %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case LOGOUT_REQUEST:

			 sprintf(szErrno, "<Logout %s>",ErrorObject.szWSAErrorMessage);

			 break;

         case RESGISTER_REQUEST:

			 sprintf(szErrno, "<Register %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case MESSAGE:

			 sprintf(szErrno, "<Send Mseeage %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case CREATEGROUP_REQUEST:

			 sprintf(szErrno, "<Create group %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case DESTROYGROUP_REQUEST:

			 sprintf(szErrno, "<Destroy group %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case JOINGROUP_REQUEST:

			 sprintf(szErrno, "<Join group reply %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case JOINGROUP_SUCCESS:
		 case JOINGROUP_FAIL:

			 sprintf(szErrno, "<Join group reply %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case LEFTGROUP_REQUEST:

			 sprintf(szErrno, "<Left group %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case LEFTGROUP_SUCCESS:
		 case LEFTGROUP_FAIL:

			 sprintf(szErrno, "<Left group reply %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case ADDGROUPMEMBER:

			 sprintf(szErrno, "<Send add group member note %s>",ErrorObject.szWSAErrorMessage);

			 break;

		 case REMOVEGROUPMEMBER:

			 sprintf(szErrno, "<Send remove group member note %s>",ErrorObject.szWSAErrorMessage);

			 break;

         case IDLE:

			 sprintf(szErrno, "<Connecting %s>",ErrorObject.szWSAErrorMessage);

			 break;
	}

	vShowText(hOutput_Listbox, szErrno);
}

void vUserLogout()
{
	int iNumberA;

	iNumberA = 0;

	while(iNumberA < MAX_SOCKET)
	{
		if(UserInfo.stSocketInfo[iNumberA].iInfo != LISTENSOCKET)
		{
			UserInfo.vCleanSocket(iNumberA);
		}

		iNumberA++;
	}

	iNumberA = 0;

	while(iNumberA < MAX_USER)
	{
		if (lpLoopBuffer[iNumberA] != NULL)
		{
			delete [] lpLoopBuffer[iNumberA];

			lpLoopBuffer[iNumberA] = NULL;
		}

		iNumberA++;
	}

	UserInfo.vInitGroupList();

	UserInfo.vInitUserList();

	UserInfo.vInitServer();

	UserInfo.vInitTarget();

	UserInfo.bUserOnLine = false;

	vLockServerInput(false);

	SetWindowText(hButton_Login,"Login");

	SetWindowText(hButton_CreateGroup,"Create Group");

	SetWindowText(hButton_JoinGroup,"Leave Group");

	vLockGroupInput(true);

	EnableWindow(hButton_Login,TRUE);

	EnableWindow(hButton_Register,TRUE);

	EnableWindow(hButton_Send,FALSE);

	SendMessage(hOutput_FListbox,LB_RESETCONTENT,0,0);

	SendMessage(hOutput_GListbox,LB_RESETCONTENT,0,0);
}

bool bGetPacket(int iSocketNo)
{
	char *szPacketTempBuffer = NULL;
	int	 iBytesReceived = 0;
	bool flag = false;
	char szBuffer[64];


	szPacketTempBuffer = new char[MAX_BUFFER];


	iBytesReceived = recv(UserInfo.stSocketInfo[iSocketNo].socket,szPacketTempBuffer,MAX_BUFFER,0);

	if (iBytesReceived == SOCKET_ERROR)
	{
		memset(szBuffer,0,sizeof(szBuffer));

		ErrorObject.vGetWSAErrorMessage(WSAGetLastError());

		sprintf(szBuffer,"<Recive Data %s>",ErrorObject.szWSAErrorMessage);

		vShowText(hOutput_Listbox, szBuffer);

		delete [] szPacketTempBuffer;

		return false;
	}

	if (UserInfo.stSocketInfo[iSocketNo].iMessageLength == 0)
	{
        memset(MessageObject.szMessageBuffer,0,sizeof(MessageObject.szMessageBuffer));

		memcpy(&UserInfo.stSocketInfo[iSocketNo].iMessageLength,szPacketTempBuffer,sizeof(int));

		if (UserInfo.stSocketInfo[iSocketNo].iMessageLength == iBytesReceived)
		{
           memcpy(&MessageObject.szMessageBuffer,szPacketTempBuffer,iBytesReceived);

		   flag = true;

		}

		if (UserInfo.stSocketInfo[iSocketNo].iMessageLength > iBytesReceived)
		{
		   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
		   {
			   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
		   }

		   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[MAX_BUFFER];

           memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,szPacketTempBuffer,iBytesReceived);

           UserInfo.stSocketInfo[iSocketNo].iReceived = iBytesReceived;

		   flag = false;
		}

		if (UserInfo.stSocketInfo[iSocketNo].iMessageLength < iBytesReceived)
		{
           memcpy(&MessageObject.szMessageBuffer,szPacketTempBuffer,UserInfo.stSocketInfo[iSocketNo].iMessageLength);

		   flag = true;
		}

	}

	else
	{
		if ((UserInfo.stSocketInfo[iSocketNo].iReceived + iBytesReceived) == UserInfo.stSocketInfo[iSocketNo].iMessageLength )
		{
           memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[UserInfo.stSocketInfo[iSocketNo].iReceived],szPacketTempBuffer,iBytesReceived);

		   memcpy(&MessageObject.szMessageBuffer, UserInfo.stSocketInfo[iSocketNo].szTempBuffer, UserInfo.stSocketInfo[iSocketNo].iMessageLength);

		   UserInfo.stSocketInfo[iSocketNo].iReceived = UserInfo.stSocketInfo[iSocketNo].iReceived + iBytesReceived;

		   iBytesReceived = 0;

		   flag = true;
		}

		if ((UserInfo.stSocketInfo[iSocketNo].iReceived + iBytesReceived) < UserInfo.stSocketInfo[iSocketNo].iMessageLength)
		{
           memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[UserInfo.stSocketInfo[iSocketNo].iReceived],szPacketTempBuffer,iBytesReceived);

           UserInfo.stSocketInfo[iSocketNo].iReceived = UserInfo.stSocketInfo[iSocketNo].iReceived + iBytesReceived;

		   iBytesReceived = 0;

		   flag = false;
		}

		if ((UserInfo.stSocketInfo[iSocketNo].iReceived + iBytesReceived) > UserInfo.stSocketInfo[iSocketNo].iMessageLength)
		{
           memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[UserInfo.stSocketInfo[iSocketNo].iReceived],szPacketTempBuffer,(UserInfo.stSocketInfo[iSocketNo].iMessageLength - UserInfo.stSocketInfo[iSocketNo].iReceived));

		   memcpy(&MessageObject.szMessageBuffer,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,UserInfo.stSocketInfo[iSocketNo].iMessageLength);

		   iBytesReceived = 0;

		   flag = true;
		}
	}

    if (szPacketTempBuffer != NULL)
    {
		delete [] szPacketTempBuffer;

	    szPacketTempBuffer = NULL;
    }

	return flag;
}

void vProcessReadMessage(int iSocketNo)
{
	struct stGroup stGroup;
	struct stUser  stUser;
	struct stUserServer stUserServer;
	struct stGroupServer stGroupList[MAX_GROUP];
	struct stUserServer  stUserList[MAX_USER];
    char szBuffer[1024];
	int iGroupID;
	int iUserID;
	int iIndex;
	int iNumberA;
	char * szToken;


	MessageObject.vGetTransMessage();

    switch(MessageObject.stTransMessage.iMessageType)
	{
	   case MESSAGE:

		   iIndex = 0;

		   iIndex = UserInfo.iFindUserFromUserName(MessageObject.stTransMessage.szSender);

		   if(UserInfo.stUser[iIndex].iSocketNO == EMPTYCONTENT)
		   {
			   UserInfo.stUser[iIndex].iSocketNO = iSocketNo;
		   }

		   iGroupID = 0;

		   memset(szBuffer,0,sizeof(szBuffer));

		   memcpy(&iGroupID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   if(iGroupID != EMPTYCONTENT)
		   {
			   sprintf(szBuffer,"<Group %s %s Says>",UserInfo.stGroup[UserInfo.iFindGroupFromGroupID(iGroupID)].szGroupName,MessageObject.stTransMessage.szSender);
		   }

		   else
		   {
			   sprintf(szBuffer,"<%s Says>",MessageObject.stTransMessage.szSender);
		   }

           vShowText(hOutput_Listbox,szBuffer);

		   memset(szBuffer,0,sizeof(szBuffer));

		   memcpy(szBuffer,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],strlen(&MessageObject.stTransMessage.szAddMessage[sizeof(int)]) + 1);

		   szToken = strtok(szBuffer,"\r\n");

		   while (szToken!= NULL)
		   {
			   vShowText(hOutput_Listbox,szToken);

			   szToken = strtok(NULL,"\r\n");
		   }

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case LOGIN_SUCCESS:

		   iUserID = 0;

		   iNumberA = 0;

		   memset(&stGroup,0,sizeof(struct stGroup));

		   memset(&stUser,0,sizeof(struct stUser));

		   memset(stGroupList,0,sizeof(stGroupList));

		   memset(stUserList,0,sizeof(stUserList));

		   memcpy(&iUserID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   UserInfo.stHost.iUserID = iUserID;

		   memcpy(stUserList,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],sizeof(stUserList));

		   while(iNumberA < MAX_USER)
		   {
			   if (stUserList[iNumberA].iUserID != EMPTYCONTENT)
			   {
				   stUser.iUserID = stUserList[iNumberA].iUserID;

		           sprintf(stUser.szUserName,stUserList[iNumberA].szUserName);

		           memcpy(&stUser.Useraddr,&stUserList[iNumberA].Useraddr,sizeof(struct sockaddr_in));

			       stUser.iIndex = EMPTYCONTENT;

			       stUser.iSocketNO = EMPTYCONTENT;

		           vAddUser(stUser);
			   }

			   memset(&stUser,0,sizeof(struct stUser));

			   iNumberA++;
		   }

		   memcpy(stGroupList,&MessageObject.stTransMessage.szAddMessage[sizeof(int)+sizeof(stUserList)],sizeof(stGroupList));

		   iNumberA = 0;

		   while(iNumberA < MAX_GROUP)
		   {
			   if (stGroupList[iNumberA].iGroupID != EMPTYCONTENT)
			   {
				   stGroup.iGroupID = stGroupList[iNumberA].iGroupID;

		           stGroup.iOwnerID = stGroupList[iNumberA].iOwnerID;

		           sprintf(stGroup.szGroupName,stGroupList[iNumberA].szGroupName);

		           sprintf(stGroup.szGroupPassWord,"");

		           memset(stGroup.iUserID,EMPTYCONTENT,sizeof(stGroup.iUserID));

			       stGroup.iIndex = EMPTYCONTENT;

		           stGroup.bJoin = false;

		           vAddGroup(stGroup);
			   }

			   memset(&stGroup,0,sizeof(struct stGroup));

			   iNumberA++;
		   }

		   UserInfo.bUserOnLine = true;

		   vLockGroupInput(false);

		   vLockServerInput(true);

		   EnableWindow(hButton_Register,FALSE);

		   EnableWindow(hButton_Login,TRUE);

		   EnableWindow(hButton_Send,TRUE);

		   SetWindowText(hButton_Login,"Logout");

		   vShowText(hOutput_Listbox,"<User login successfully>");

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case LOGIN_FAIL:

		   sprintf(szBuffer,"<Login fail : %s>",MessageObject.stTransMessage.szAddMessage);

		   vShowText(hOutput_Listbox,szBuffer);

		   UserInfo.vCleanSocket(iSocketNo);

		   UserInfo.stServer.iSocketNO = EMPTYCONTENT;

		   vLockServerInput(false);

		   EnableWindow(hButton_Login,TRUE);

		   EnableWindow(hButton_Register,TRUE);

		   break;

	   case LOGOUT_SUCCESS:

		   vUserLogout();

		   vShowText(hOutput_Listbox,"<User logout successfully>");

		   break;

	   case RESGISTER_SUCCESS:

		   vShowText(hOutput_Listbox,"<User Register Successfully>");

		   UserInfo.vCleanSocket(iSocketNo);

		   UserInfo.stServer.iSocketNO = EMPTYCONTENT;

		   vLockServerInput(false);

		   EnableWindow(hButton_Login,TRUE);

		   EnableWindow(hButton_Register,TRUE);

		   break;

	   case RESGISTER_FAIL:

		   sprintf(szBuffer,"<Register fail : %s>",MessageObject.stTransMessage.szAddMessage);

		   vShowText(hOutput_Listbox,szBuffer);

		   UserInfo.vCleanSocket(iSocketNo);

		   UserInfo.stServer.iSocketNO = EMPTYCONTENT;

		   vLockServerInput(false);

		   EnableWindow(hButton_Login,TRUE);

		   EnableWindow(hButton_Register,TRUE);

		   break;

	   case CREATEGROUP_SUCCESS:

		   iGroupID = 0;

		   memset(&stGroup,0,sizeof(struct stGroup));

		   GetWindowText(hInput_GroupName,szBuffer,16);

		   sprintf(stGroup.szGroupName,szBuffer);

		   GetWindowText(hInput_GroupPassword,szBuffer,16);

		   sprintf(stGroup.szGroupPassWord,szBuffer);

		   memcpy(&iGroupID, MessageObject.stTransMessage.szAddMessage, sizeof(int));

		   stGroup.iGroupID = iGroupID;

		   stGroup.iOwnerID = UserInfo.stHost.iUserID;

		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   stGroup.iUserID[iNumberA] = EMPTYCONTENT;

               iNumberA++;
		   }

		   stGroup.iUserID[0] = UserInfo.stHost.iUserID;

		   stGroup.bJoin = true;

		   stGroup.iIndex = EMPTYCONTENT;

		   vAddGroup(stGroup);

		   vLockGroupInput(false);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case CREATEGROUP_FAIL:

		   sprintf(szBuffer,"<Create group fail : %s>",MessageObject.stTransMessage.szAddMessage);

		   vShowText(hOutput_Listbox,szBuffer);

		   vLockGroupInput(false);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case DESTROYGROUP_SUCCESS:

		   iGroupID = 0;

		   memcpy(&iGroupID, MessageObject.stTransMessage.szAddMessage, sizeof(int));

		   if(UserInfo.stTarget.iTargetType == TOGROUP && UserInfo.stTarget.iIndex == UserInfo.stGroup[UserInfo.iFindGroupFromGroupID(iGroupID)].iIndex)
		   {
			   vShowUser();
		   }

		   vRemoveGroup(iGroupID);

		   vLockGroupInput(false);

		   SetWindowText(hButton_CreateGroup,"Create Group");

		   vShowText(hOutput_Listbox,"<Destory group successfully>");

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case ADDUSER:

		   memset(&stUser,0,sizeof(struct stUser));

		   memset(&stUserServer,0,sizeof(struct stUserServer));

		   memcpy(&stUserServer, MessageObject.stTransMessage.szAddMessage, sizeof(struct stUserServer));

		   stUser.iIndex = EMPTYCONTENT;

		   stUser.iSocketNO = EMPTYCONTENT;

		   stUser.iUserID = stUserServer.iUserID;

		   sprintf(stUser.szUserName,stUserServer.szUserName);

		   memcpy(&stUser.Useraddr,&stUserServer.Useraddr,sizeof(struct sockaddr_in));

		   vAddUser(stUser);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case REMOVEUSER:

		   iUserID = 0;

		   memcpy(&iUserID, MessageObject.stTransMessage.szAddMessage, sizeof(int));

		   vRemoveUser(iUserID);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case ADDGROUP:

		   iGroupID = 0;

		   iUserID = 0;

		   memset(&stGroup,0,sizeof(struct stGroup));

		   memcpy(&iGroupID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   memcpy(&iUserID,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],sizeof(int));

		   memcpy(&szBuffer,&MessageObject.stTransMessage.szAddMessage[(sizeof(int))*2],16);

		   stGroup.iGroupID = iGroupID;

		   stGroup.iOwnerID = iUserID;

		   sprintf(stGroup.szGroupName,szBuffer);

		   sprintf(stGroup.szGroupPassWord,"");

		   memset(stGroup.iUserID,EMPTYCONTENT,sizeof(stGroup.iUserID));

		   stGroup.bJoin = false;

		   stGroup.iIndex = EMPTYCONTENT;

		   vAddGroup(stGroup);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case REMOVEGROUP:

		   iGroupID = 0;

		   memcpy(&iGroupID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   if(UserInfo.stTarget.iTargetType == TOGROUP && UserInfo.stTarget.iIndex == UserInfo.stGroup[UserInfo.iFindGroupFromGroupID(iGroupID)].iIndex)
		   {
			   vShowUser();
		   }

		   vRemoveGroup(iGroupID);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case JOINGROUP_REQUEST:

		   iGroupID = 0;

		   iIndex = 0;

		   iIndex = UserInfo.iFindUserFromUserName(MessageObject.stTransMessage.szSender);

		   if(UserInfo.stUser[iIndex].iSocketNO == EMPTYCONTENT)
		   {
			   UserInfo.stUser[iIndex].iSocketNO = iSocketNo;
		   }

		   iIndex = 0;

		   memcpy(&iGroupID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   memcpy(szBuffer,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],32);

           iIndex = UserInfo.iFindGroupFromGroupID(iGroupID);

		   if (UserInfo.stGroup[iIndex].iOwnerID != UserInfo.stHost.iUserID)
		   {
			   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }

			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [32];

			   sprintf(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,"Invalid request");

			   UserInfo.stSocketInfo[iSocketNo].iInfo = JOINGROUP_FAIL;
		   }

		   else
		   {
			   if(strcmp(szBuffer,UserInfo.stGroup[iIndex].szGroupPassWord) != 0)
			   {
				   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
				   {
					   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				       UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
				   }

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [32];

				   sprintf(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,"Invalid password");

			       UserInfo.stSocketInfo[iSocketNo].iInfo = JOINGROUP_FAIL;
			   }

			   else
			   {
				   iUserID = 0;

				   iNumberA = 0;

		           while(lpLoopBuffer[iNumberA] != NULL)
				   {
					   iNumberA++;
				   }

				   lpLoopBuffer[iNumberA] = new char[1024];

				   iUserID = UserInfo.stUser[UserInfo.iFindUserFromSocketNo(iSocketNo)].iUserID;

				   memcpy(lpLoopBuffer[iNumberA],&iUserID,sizeof(int));

				   memcpy(lpLoopBuffer[iNumberA] + sizeof(int),&iGroupID,sizeof(int));

				   memcpy(lpLoopBuffer[iNumberA] + sizeof(int)*2,UserInfo.stGroup[iIndex].iUserID,sizeof(UserInfo.stGroup[iIndex].iUserID));

				   PostMessage(hMain_Window,WM_LOOP,ADDGROUPMEMBER,(LPARAM)lpLoopBuffer[iNumberA]);

				   iNumberA = 0;

				   while(iNumberA<MAX_USER && UserInfo.stGroup[iIndex].iUserID[iNumberA] != EMPTYCONTENT)
				   {
					   iNumberA++;
				   }

				   UserInfo.stGroup[iIndex].iUserID[iNumberA] = iUserID;

				   UserInfo.stSocketInfo[iSocketNo].iInfo = JOINGROUP_SUCCESS;

				   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
				   {
					   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				       UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
				   }

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[sizeof(int) + sizeof(UserInfo.stGroup[iIndex].iUserID)];

				   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iGroupID, sizeof(int));

				   memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)],UserInfo.stGroup[iIndex].iUserID, sizeof(UserInfo.stGroup[iIndex].iUserID));

				   memset(szBuffer,0,sizeof(szBuffer));

		           sprintf(szBuffer,"<User %s Join Group %s>",UserInfo.stUser[UserInfo.iFindUserFromUserID(iUserID)].szUserName,UserInfo.stGroup[iIndex].szGroupName);

		           vShowText(hOutput_Listbox,szBuffer);

				   if(UserInfo.stTarget.iTargetType == TOGROUP && UserInfo.stTarget.iIndex == UserInfo.stGroup[iIndex].iIndex)
				   {
					   vShowGroupMember(iGroupID);
				   }
			   }
		   }

		   break;

	   case JOINGROUP_SUCCESS:

		   iGroupID = 0;

		   iIndex = 0;

		   memset(szBuffer,0,sizeof(szBuffer));

		   memcpy(&iGroupID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   iIndex = UserInfo.iFindGroupFromGroupID(iGroupID);

		   memcpy(UserInfo.stGroup[iIndex].iUserID,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],sizeof(UserInfo.stGroup[iIndex].iUserID));

		   UserInfo.stGroup[iIndex].bJoin = true;

		   sprintf(szBuffer,"J %s",UserInfo.stGroup[iIndex].szGroupName);

           SendMessage(hOutput_GListbox,LB_DELETESTRING,UserInfo.stGroup[iIndex].iIndex,0);

           SendMessage(hOutput_GListbox,LB_INSERTSTRING,UserInfo.stGroup[iIndex].iIndex,(LPARAM)szBuffer);

		   memset(szBuffer,0,sizeof(szBuffer));

           sprintf(szBuffer,"<Group %s Joined>",UserInfo.stGroup[iIndex].szGroupName);

		   vShowText(hOutput_Listbox,szBuffer);

		   vLockGroupInput(false);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   if(UserInfo.stTarget.iTargetType == TOGROUP && UserInfo.stTarget.iIndex == UserInfo.stGroup[iIndex].iIndex)
		   {
			   vShowGroupMember(iGroupID);
		   }

		   break;

	   case JOINGROUP_FAIL:

		   memset(szBuffer,0,sizeof(szBuffer));

		   sprintf(szBuffer,"<Join group fail : %s>",MessageObject.stTransMessage.szAddMessage);

		   vShowText(hOutput_Listbox,szBuffer);

		   vLockGroupInput(false);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case LEFTGROUP_REQUEST:

		   iGroupID = 0;

		   iUserID = 0;

		   iNumberA = 0;

		   memset(szBuffer,0,sizeof(szBuffer));

		   iIndex = 0;

		   iIndex = UserInfo.iFindUserFromUserName(MessageObject.stTransMessage.szSender);

		   if(UserInfo.stUser[iIndex].iSocketNO == EMPTYCONTENT)
		   {
			   UserInfo.stUser[iIndex].iSocketNO = iSocketNo;
		   }

		   memcpy(&iGroupID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   iIndex = 0;

		   iIndex = UserInfo.iFindGroupFromGroupID(iGroupID);

		   if(UserInfo.stGroup[iIndex].iOwnerID == UserInfo.stHost.iUserID)
		   {
			   iUserID = UserInfo.stUser[UserInfo.iFindUserFromSocketNo(iSocketNo)].iUserID;

			   while(iNumberA<MAX_USER && UserInfo.stGroup[iIndex].iUserID[iNumberA] != iUserID)
			   {
				   iNumberA++;
			   }

			   UserInfo.stGroup[iIndex].iUserID[iNumberA] = EMPTYCONTENT;

			   memcpy(szBuffer,&iUserID,sizeof(int));

			   memcpy(&szBuffer[sizeof(int)],&iGroupID,sizeof(int));

			   memcpy(&szBuffer[sizeof(int)*2],UserInfo.stGroup[iIndex].iUserID,sizeof(UserInfo.stGroup[iIndex].iUserID));

			   SendMessage(hMain_Window,WM_LOOP,REMOVEGROUPMEMBER,(LPARAM)szBuffer);

			   UserInfo.stSocketInfo[iSocketNo].iInfo = LEFTGROUP_SUCCESS;

			   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }

			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[sizeof(int)];

			   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iGroupID, sizeof(int));

			   memset(szBuffer,0,sizeof(szBuffer));

		       sprintf(szBuffer,"<User %s Left Group %s>",UserInfo.stUser[UserInfo.iFindUserFromUserID(iUserID)].szUserName,UserInfo.stGroup[iIndex].szGroupName);

		       vShowText(hOutput_Listbox,szBuffer);

			   if(UserInfo.stTarget.iTargetType == TOGROUP && UserInfo.stTarget.iIndex == UserInfo.stGroup[iIndex].iIndex)
			   {
				   vShowGroupMember(iGroupID);
			   }
		   }

		   break;

	   case LEFTGROUP_SUCCESS:

		   iGroupID = 0;

		   iIndex = 0;

		   iNumberA = 0;

		   memset(szBuffer,0,sizeof(szBuffer));

		   memcpy(&iGroupID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   iIndex = UserInfo.iFindGroupFromGroupID(iGroupID);

		   while(iNumberA<MAX_USER)
		   {
			   UserInfo.stGroup[iIndex].iUserID[iNumberA] = EMPTYCONTENT;

			   iNumberA++;
		   }

		   UserInfo.stGroup[iIndex].bJoin = false;

		   sprintf(szBuffer,"  %s",UserInfo.stGroup[iIndex].szGroupName);

           SendMessage(hOutput_GListbox,LB_DELETESTRING,UserInfo.stGroup[iIndex].iIndex,0);

           SendMessage(hOutput_GListbox,LB_INSERTSTRING,UserInfo.stGroup[iIndex].iIndex,(LPARAM)szBuffer);

		   vLockGroupInput(false);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   memset(szBuffer,0,sizeof(szBuffer));

		   sprintf(szBuffer,"<Group %s Left>",UserInfo.stGroup[iIndex].szGroupName);

		   vShowText(hOutput_Listbox,szBuffer);

		   if(UserInfo.stTarget.iTargetType == TOGROUP && UserInfo.stTarget.iIndex == UserInfo.stGroup[iIndex].iIndex)
		   {
			   vShowUser();
		   }

		   break;

	   case LEFTGROUP_FAIL:

		   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
		   {
			   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
		   }

		   vLockGroupInput(false);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   break;

	   case ADDGROUPMEMBER:

		   iGroupID = 0;

		   iIndex = 0;

		   iUserID = 0;

		   iNumberA = 0;

		   memset(szBuffer,0,sizeof(szBuffer));

		   memcpy(&iUserID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   memcpy(&iGroupID,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],sizeof(int));

		   iIndex = UserInfo.iFindGroupFromGroupID(iGroupID);

		   while(iNumberA<MAX_USER && UserInfo.stGroup[iIndex].iUserID[iNumberA] != EMPTYCONTENT)
		   {
			   iNumberA++;
		   }

		   UserInfo.stGroup[iIndex].iUserID[iNumberA] = iUserID;

		   memset(szBuffer,0,sizeof(szBuffer));

		   sprintf(szBuffer,"<User %s Join Group %s>",UserInfo.stUser[UserInfo.iFindUserFromUserID(iUserID)].szUserName,UserInfo.stGroup[iIndex].szGroupName);

		   vShowText(hOutput_Listbox,szBuffer);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   if(UserInfo.stTarget.iTargetType == TOGROUP && UserInfo.stTarget.iIndex == UserInfo.stGroup[iIndex].iIndex)
		   {
			   vShowGroupMember(iGroupID);
		   }

		   break;

	   case REMOVEGROUPMEMBER:

		   iGroupID = 0;

		   iIndex = 0;

		   iUserID = 0;

		   iNumberA = 0;

		   memset(szBuffer,0,sizeof(szBuffer));

		   memcpy(&iUserID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   memcpy(&iGroupID,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],sizeof(int));

		   iIndex = UserInfo.iFindGroupFromGroupID(iGroupID);

		   while(iNumberA<MAX_USER && UserInfo.stGroup[iIndex].iUserID[iNumberA] != iUserID)
		   {
			   iNumberA++;
		   }

		   UserInfo.stGroup[iIndex].iUserID[iNumberA] = EMPTYCONTENT;

		   UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

		   memset(szBuffer,0,sizeof(szBuffer));

		   sprintf(szBuffer,"<User %s Left Group %s>",UserInfo.stUser[UserInfo.iFindUserFromUserID(iUserID)].szUserName,UserInfo.stGroup[iIndex].szGroupName);

		   vShowText(hOutput_Listbox,szBuffer);

		   if(UserInfo.stTarget.iTargetType == TOGROUP && UserInfo.stTarget.iIndex == UserInfo.stGroup[iIndex].iIndex)
		   {
			   vShowGroupMember(iGroupID);
		   }

		   break;

	}
}


void vAddUser(struct stUser stAddUser)
{
	int iNumberA;
	int iLine;
	char szBuffer[64];

    iNumberA = 0;

	iLine = 0;

	while(iNumberA < MAX_USER && UserInfo.stUser[iNumberA].iUserID != EMPTYCONTENT)
	{
		iNumberA++;
	}

	UserInfo.stUser[iNumberA].iSocketNO = EMPTYCONTENT;

	UserInfo.stUser[iNumberA].iUserID = stAddUser.iUserID;

	sprintf(UserInfo.stUser[iNumberA].szUserName,stAddUser.szUserName);

    memcpy(&UserInfo.stUser[iNumberA].Useraddr,&stAddUser.Useraddr,sizeof(struct sockaddr_in));

    SendMessage(hOutput_FListbox,LB_ADDSTRING,0,(LPARAM)stAddUser.szUserName);

	iLine = SendMessage(hOutput_FListbox,LB_GETCOUNT,0,0);

	UserInfo.stUser[iNumberA].iIndex = iLine - 1;

	memset(szBuffer,0,sizeof(szBuffer));

    sprintf(szBuffer,"<User %s Online>",UserInfo.stUser[iNumberA].szUserName);

    vShowText(hOutput_Listbox,szBuffer);
}

void vRemoveUser(int iUserID)
{
	int iNumberA;
	int iNumberB;
	int iNumberC;
	char szBuffer[64];

    iNumberA = 0;

	iNumberB = 0;

	iNumberA = UserInfo.iFindUserFromUserID(iUserID);

	if (iNumberA != EMPTYCONTENT)
	{
		if(UserInfo.stTarget.iTargetType == TOUSER)
		{
			if(UserInfo.stTarget.iIndex == UserInfo.stUser[iNumberA].iIndex)
			{
				SetWindowText(hText_TalkTo,"Talk To");

				UserInfo.stTarget.iTargetType = TOSERVER;
			}
			if(UserInfo.stTarget.iIndex > UserInfo.stUser[iNumberA].iIndex)
			{
				UserInfo.stTarget.iIndex--;
			}
		}
		SendMessage(hOutput_FListbox,LB_DELETESTRING,UserInfo.stUser[iNumberA].iIndex,0);

		while(iNumberB<MAX_USER)
		{
			if (UserInfo.stUser[iNumberB].iIndex > UserInfo.stUser[iNumberA].iIndex)
			{
				UserInfo.stUser[iNumberB].iIndex = UserInfo.stUser[iNumberB].iIndex - 1;
			}

			iNumberB++;
		}

		UserInfo.stUser[iNumberA].iIndex = EMPTYCONTENT;

		iNumberB = 0;

		while(iNumberB<MAX_GROUP)
		{
			if (UserInfo.stGroup[iNumberB].iOwnerID == iUserID)
			{
				vRemoveGroup(UserInfo.stGroup[iNumberB].iGroupID);
			}

			else
			{
				if (UserInfo.stGroup[iNumberB].iOwnerID != EMPTYCONTENT)
				{
					iNumberC = 0;

				    while(iNumberC<MAX_USER)
					{
						if(UserInfo.stGroup[iNumberB].iUserID[iNumberC] == iUserID)
						{
							UserInfo.stGroup[iNumberB].iUserID[iNumberC] = EMPTYCONTENT;
						}
						iNumberC++;
					}
				}
			}

			iNumberB++;
		}

		memset(szBuffer,0,sizeof(szBuffer));

		sprintf(szBuffer,"<User %s Offline>",UserInfo.stUser[iNumberA].szUserName);

        vShowText(hOutput_Listbox,szBuffer);

		UserInfo.stUser[iNumberA].iUserID = EMPTYCONTENT;

		sprintf(UserInfo.stUser[iNumberA].szUserName,"");

		memset(&UserInfo.stUser[iNumberA].Useraddr,0,sizeof(sockaddr_in));

		if(UserInfo.stUser[iNumberA].iSocketNO != EMPTYCONTENT)
		{
			UserInfo.vCleanSocket(UserInfo.stUser[iNumberA].iSocketNO);

			UserInfo.stUser[iNumberA].iSocketNO = EMPTYCONTENT;
		}
	}
}

void vAddGroup(struct stGroup stAddGroup)
{
	int iNumberA;
	int iLine;
	char szBuffer[64];

    iNumberA = 0;

	iLine = 0;

	while(iNumberA < MAX_GROUP && UserInfo.stGroup[iNumberA].iGroupID != EMPTYCONTENT)
	{
		iNumberA++;
	}

	UserInfo.stGroup[iNumberA].iGroupID = stAddGroup.iGroupID;

	UserInfo.stGroup[iNumberA].iOwnerID = stAddGroup.iOwnerID;

	sprintf(UserInfo.stGroup[iNumberA].szGroupName,stAddGroup.szGroupName);

	sprintf(UserInfo.stGroup[iNumberA].szGroupPassWord,stAddGroup.szGroupPassWord);

    memcpy(UserInfo.stGroup[iNumberA].iUserID,stAddGroup.iUserID,sizeof(stAddGroup.iUserID));

	if(stAddGroup.iOwnerID == UserInfo.stHost.iUserID)
	{
		UserInfo.stGroup[iNumberA].bJoin = true;

		sprintf(szBuffer,"O %s",stAddGroup.szGroupName);
	}

	else
	{
		UserInfo.stGroup[iNumberA].bJoin = false;

		sprintf(szBuffer,"  %s",stAddGroup.szGroupName);
	}

	SendMessage(hOutput_GListbox,LB_ADDSTRING,0,(LPARAM)szBuffer);

	iLine = SendMessage(hOutput_GListbox,LB_GETCOUNT,0,0);

	UserInfo.stGroup[iNumberA].iIndex = iLine - 1;

	memset(szBuffer,0,sizeof(szBuffer));

    sprintf(szBuffer,"<Group %s Created>",UserInfo.stGroup[iNumberA].szGroupName);

    vShowText(hOutput_Listbox,szBuffer);
}

void vRemoveGroup(int iGroupID)
{
	int iNumberA;
	int iNumberB;
	char szBuffer[64];

    iNumberA = 0;

	iNumberB = 0;

	iNumberA = UserInfo.iFindGroupFromGroupID(iGroupID);

	if (iNumberA != EMPTYCONTENT)
	{
		if(UserInfo.stTarget.iTargetType == TOGROUP)
		{
			if(UserInfo.stTarget.iIndex == UserInfo.stGroup[iNumberA].iIndex)
			{
				SetWindowText(hText_TalkTo,"Talk To");

				UserInfo.stTarget.iTargetType = TOSERVER;
			}

			if(UserInfo.stTarget.iIndex > UserInfo.stGroup[iNumberA].iIndex)
			{
				UserInfo.stTarget.iIndex--;
			}
		}

		SendMessage(hOutput_GListbox,LB_DELETESTRING,UserInfo.stGroup[iNumberA].iIndex,0);

		while(iNumberB<MAX_GROUP)
		{
			if (UserInfo.stGroup[iNumberB].iIndex > UserInfo.stGroup[iNumberA].iIndex)
			{
				UserInfo.stGroup[iNumberB].iIndex = UserInfo.stGroup[iNumberB].iIndex - 1;
			}

			iNumberB++;
		}

		memset(szBuffer,0,sizeof(szBuffer));

        sprintf(szBuffer,"<Group %s Destroyed>",UserInfo.stGroup[iNumberA].szGroupName);

        vShowText(hOutput_Listbox,szBuffer);

		UserInfo.stGroup[iNumberA].iIndex = EMPTYCONTENT;

		UserInfo.stGroup[iNumberA].iGroupID = EMPTYCONTENT;

		UserInfo.stGroup[iNumberA].iOwnerID = EMPTYCONTENT;

		UserInfo.stGroup[iNumberA].bJoin = false;

		sprintf(UserInfo.stGroup[iNumberA].szGroupName,"");

		sprintf(UserInfo.stGroup[iNumberA].szGroupPassWord,"");

		iNumberB = 0;

		while(iNumberB < MAX_USER)
		{
			UserInfo.stGroup[iNumberA].iUserID[iNumberB] = EMPTYCONTENT;

			iNumberB++;
		}
	}
}

void vShowGroupMember(int iGroupID)
{
	int iNumberA;
	int iNumberB;
	int iGroupIndex;
	bool bFlag;
	char szBuffer[32];

	iNumberA = 0;

	iGroupIndex = UserInfo.iFindGroupFromGroupID(iGroupID);

	while(iNumberA < MAX_USER)
	{
		iNumberB = 0;

		bFlag = false;

		while(iNumberB < MAX_USER && !bFlag)
		{
			if(UserInfo.stGroup[iGroupIndex].iUserID[iNumberB] == UserInfo.stUser[iNumberA].iUserID)
			{
				bFlag = true;
			}
			else
			{
				iNumberB++;
			}
		}

		if(bFlag)
		{
			memset(szBuffer,0,sizeof(szBuffer));

			sprintf(szBuffer,"M %s",UserInfo.stUser[iNumberA].szUserName);

			SendMessage(hOutput_FListbox,LB_DELETESTRING,UserInfo.stUser[iNumberA].iIndex,0);

			SendMessage(hOutput_FListbox,LB_INSERTSTRING,UserInfo.stUser[iNumberA].iIndex,(LPARAM)szBuffer);
		}

		else
		{
			if(UserInfo.stUser[iNumberA].iIndex != EMPTYCONTENT)
			{
				SendMessage(hOutput_FListbox,LB_DELETESTRING,UserInfo.stUser[iNumberA].iIndex,0);

			    SendMessage(hOutput_FListbox,LB_INSERTSTRING,UserInfo.stUser[iNumberA].iIndex,(LPARAM)UserInfo.stUser[iNumberA].szUserName);
			}
		}

		iNumberA++;
	}
}

void vShowUser()
{
	int iNumberA;

	iNumberA = 0;

	while(iNumberA < MAX_USER)
	{
		if(UserInfo.stUser[iNumberA].iIndex != EMPTYCONTENT)
		{
			SendMessage(hOutput_FListbox,LB_DELETESTRING,UserInfo.stUser[iNumberA].iIndex,0);

		    SendMessage(hOutput_FListbox,LB_INSERTSTRING,UserInfo.stUser[iNumberA].iIndex,(LPARAM)UserInfo.stUser[iNumberA].szUserName);
		}

		iNumberA++;
	}

	SendMessage(hOutput_FListbox,LB_SETCURSEL,UserInfo.stTarget.iIndex,0);
}
