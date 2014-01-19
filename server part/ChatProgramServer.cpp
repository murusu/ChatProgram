#include "ChatProgramServer.h"
#include "User.h"
#include "error.h"

UserObject    UserInfo;
MessageObject MessageObject;
ErrorObject   ErrorObject;

int           iSendToGroupList[MAX_USER];
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
	int         iSocketNo;
	int         iNumberA;

	switch (iMsg)
    {
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{				
				case IDC_Button_StartUp:

					if(UserInfo.bServerOnLine == true)
					{
						vServerShutdown();

						vShowText(hOutput_Listbox,"<Server Shutdown successfully>");
					}

					else
					{
						if(bServerStartUp())
						{
							vShowText(hOutput_Listbox,"<Server Start-up successfully>");
						}
						else
						{
							vShowText(hOutput_Listbox,"<Server Start-up fail>");
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

			int iUserID;
			int iGroupID;
			int iIndex;
			bool bLeft;
			char szGroupName[INFO_LENGTH];
			struct stUserServer stUserServer;
			int iUserIDList[MAX_USER];

			iUserID = 0;

			iGroupID = 0;

			iNumberA = 0;

			iIndex = 0;

			iSocketNo = 0;

			bLeft = false;

			memset(szGroupName,0,sizeof(szGroupName));


			switch (wParam)
			{
			   case ADDUSER:

				   memcpy(&stUserServer,(char *)lParam,sizeof(stUserServer));

				   memcpy(iUserIDList,(char *)lParam + sizeof(stUserServer),sizeof(iUserIDList));
				   
				   while(iNumberA < MAX_USER)
				   {
					   if(iUserIDList[iNumberA] != EMPTYCONTENT)
					   {
						   iIndex = UserInfo.iFindUserFromUserID(iUserIDList[iNumberA]);

					       iSocketNo = UserInfo.stUser[iIndex].iSocketNO;

					       if(UserInfo.stSocketInfo[iSocketNo].iInfo == IDLE)
						   {
							   UserInfo.stSocketInfo[iSocketNo].iInfo = ADDUSER;

							   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
							   {
								   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				                   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
							   }
							   
							   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[sizeof(stUserServer)];
							   
							   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&stUserServer,sizeof(stUserServer));

                               if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
							   {
								   vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

	                               break;
							   }

                               iUserIDList[iNumberA] = EMPTYCONTENT;
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
					   memcpy((char *)lParam,&stUserServer,sizeof(stUserServer));

					   memcpy((char *)lParam + sizeof(stUserServer),iUserIDList,sizeof(iUserIDList));

					   PostMessage(hWnd,WM_LOOP,ADDUSER,lParam);
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

			   case REMOVEUSER:

				   memcpy(&iUserID,(char *)lParam,sizeof(int));

				   memcpy(iUserIDList,(char *)lParam + sizeof(int),sizeof(iUserIDList));
				   
				   while(iNumberA < MAX_USER)
				   {
					   if(iUserIDList[iNumberA] != EMPTYCONTENT)
					   {
						   iIndex = UserInfo.iFindUserFromUserID(iUserIDList[iNumberA]);

					       iSocketNo = UserInfo.stUser[iIndex].iSocketNO;

					       if(UserInfo.stSocketInfo[iSocketNo].iInfo == IDLE)
						   {
							   UserInfo.stSocketInfo[iSocketNo].iInfo = REMOVEUSER;

							   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
							   {
								   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				                   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
							   }
							   
							   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[sizeof(int)];
							   
							   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iUserID,sizeof(int));
                               
							   if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
							   {
								   vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

	                               break;
							   }

							   iUserIDList[iNumberA] = EMPTYCONTENT;
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

				       memcpy((int *)lParam + 1,iUserIDList,sizeof(iUserIDList));
				   }

				   else
				   {
					   delete [] (int *)lParam;

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

			   case ADDGROUP:

				   memcpy(&iGroupID,(char *)lParam,sizeof(int));

				   memcpy(&iUserID,(char *)lParam + sizeof(int),sizeof(int));

				   memcpy(szGroupName,(char *)lParam + sizeof(int)*2,sizeof(szGroupName));

                   memcpy(iUserIDList,(char *)lParam + sizeof(int)*2 + sizeof(szGroupName),sizeof(iUserIDList));

				   while(iNumberA < MAX_USER)
				   {
					   if(iUserIDList[iNumberA] != EMPTYCONTENT)
					   {
						   iIndex = UserInfo.iFindUserFromUserID(iUserIDList[iNumberA]);

					       iSocketNo = UserInfo.stUser[iIndex].iSocketNO;

					       if(UserInfo.stSocketInfo[iSocketNo].iInfo == IDLE)
						   {
							   UserInfo.stSocketInfo[iSocketNo].iInfo = ADDGROUP;

							   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
							   {
								   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				                   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
							   }
							   
							   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[sizeof(int)*2 + sizeof(szGroupName)];
							   
							   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iGroupID,sizeof(int));

							   memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)],&iUserID,sizeof(int));

							   memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)*2],szGroupName,sizeof(szGroupName));

                               if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
							   {
								   vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

	                               break;
							   }

							   iUserIDList[iNumberA] = EMPTYCONTENT;
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

					   memcpy((char *)lParam + sizeof(int),&iUserID,sizeof(int));

					   memcpy((char *)lParam + sizeof(int)*2,szGroupName,sizeof(szGroupName));

				       memcpy((char *)lParam + sizeof(int)*2 + sizeof(szGroupName),iUserIDList,sizeof(iUserIDList));
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

			   case REMOVEGROUP:

				   memcpy(&iGroupID,(char *)lParam,sizeof(int));

                   memcpy(iUserIDList,(char *)lParam + sizeof(int),sizeof(iUserIDList));

				   while(iNumberA < MAX_USER)
				   {
					   if(iUserIDList[iNumberA] != EMPTYCONTENT)
					   {
						   iIndex = UserInfo.iFindUserFromUserID(iUserIDList[iNumberA]);

					       iSocketNo = UserInfo.stUser[iIndex].iSocketNO;

					       if(UserInfo.stSocketInfo[iSocketNo].iInfo == IDLE)
						   {
							   UserInfo.stSocketInfo[iSocketNo].iInfo = REMOVEGROUP;

							   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
							   {
								   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				                   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
							   }
							   
							   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char[sizeof(int)];
							   
							   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iGroupID,sizeof(int));

                               if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
							   {
								   vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
								   
								   break;
							   }


							   iUserIDList[iNumberA] = EMPTYCONTENT;
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

				       memcpy((char *)lParam + sizeof(int),iUserIDList,sizeof(iUserIDList));
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

			  iSocketNo = 0;

			  while (wParam != UserInfo.stSocketInfo[iSocketNo].socket)
			  {
				  iSocketNo++;
			  }

			  if (WSAGETSELECTERROR(lParam))
			  {
				  ErrorObject.vGetWSAErrorMessage(WSAGETSELECTERROR(lParam));

				  vShowSocketError(iSocketNo);
 
				  UserInfo.vCleanSocket(iSocketNo);

				  break;
			  }

			switch (WSAGETSELECTEVENT(lParam))
			{
			  case FD_ACCEPT:

				  iSocketNo = UserInfo.iGetSocket();

				  UserInfo.stSocketInfo[iSocketNo].socket = socket(AF_INET, SOCK_STREAM, 0);

				  UserInfo.stSocketInfo[iSocketNo].socket = accept(wParam,NULL,NULL);

				  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_CLOSE))
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
//UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;

					  vProcessReadMessage(iSocketNo);

					  UserInfo.stSocketInfo[iSocketNo].iMessageLength = 0 ;
					  
					  if (WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
					  {
						  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

						  break;
					  }
				  }

				  break;

			  case FD_WRITE:

				  switch (UserInfo.stSocketInfo[iSocketNo].iInfo)
				  {
				      case LOGIN_SUCCESS:

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  sprintf(MessageObject.stTransMessage.szSender,"Server");

						  MessageObject.stTransMessage.iMessageType = LOGIN_SUCCESS;

						  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int) + sizeof(stUserServer)*MAX_USER + sizeof(stGroupServer)*MAX_GROUP);

						  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

						  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender) + sizeof(stUserServer)*MAX_USER + sizeof(stGroupServer)*MAX_GROUP;

						  MessageObject.vSetTransMessage();

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  break;

				      case LOGIN_FAIL:

                          memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  sprintf(MessageObject.stTransMessage.szSender,"Server");

						  MessageObject.stTransMessage.iMessageType = LOGIN_FAIL;

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
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {
							  
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  break;

				      case LOGOUT_SUCCESS:
						  
						  sprintf(MessageObject.stTransMessage.szSender,"Server");

						  MessageObject.stTransMessage.iMessageType = LOGOUT_SUCCESS;

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender);

						  MessageObject.vSetTransMessage();

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  break;

				      case RESGISTER_SUCCESS:
						  
						  sprintf(MessageObject.stTransMessage.szSender,"Server");

						  MessageObject.stTransMessage.iMessageType = RESGISTER_SUCCESS;

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender);

						  MessageObject.vSetTransMessage();

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  break;

				      case RESGISTER_FAIL:

						  sprintf(MessageObject.stTransMessage.szSender,"Server");

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  MessageObject.stTransMessage.iMessageType = RESGISTER_FAIL;

						  if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						  {
							  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,strlen(UserInfo.stSocketInfo[iSocketNo].szTempBuffer));

							  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						  }

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender) + strlen(MessageObject.stTransMessage.szAddMessage) + 1;

						  MessageObject.vSetTransMessage();

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  break;

				      case CREATEGROUP_SUCCESS:

                          sprintf(MessageObject.stTransMessage.szSender,"Server");

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  MessageObject.stTransMessage.iMessageType = CREATEGROUP_SUCCESS;

						  if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						  {
							  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int));

							  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						  }

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender);

						  MessageObject.vSetTransMessage();

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

	                          break;
						  }

						  break;

				      case CREATEGROUP_FAIL:

                          sprintf(MessageObject.stTransMessage.szSender,"Server");

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  MessageObject.stTransMessage.iMessageType = CREATEGROUP_FAIL;

						  if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						  {
							  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,strlen(UserInfo.stSocketInfo[iSocketNo].szTempBuffer));

							  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						  }

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender) + strlen(MessageObject.stTransMessage.szAddMessage);

						  MessageObject.vSetTransMessage();

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {							  
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }
						  break;

				      case DESTROYGROUP_SUCCESS:

                          sprintf(MessageObject.stTransMessage.szSender,"Server");

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  MessageObject.stTransMessage.iMessageType = DESTROYGROUP_SUCCESS;

						  if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						  {
							  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int));

							  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						  }

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender);

						  MessageObject.vSetTransMessage();

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {
							  
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  break;

				      case ADDUSER:

                          sprintf(MessageObject.stTransMessage.szSender,"Server");

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  MessageObject.stTransMessage.iMessageType = ADDUSER;

						  if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						  {
							  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(stUserServer));

							  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						  }

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*2 + sizeof(MessageObject.stTransMessage.szSender) + sizeof(stUserServer);

						  MessageObject.vSetTransMessage();

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {							  
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  break;

				      case REMOVEUSER:						  

                          sprintf(MessageObject.stTransMessage.szSender,"Server");

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  MessageObject.stTransMessage.iMessageType = REMOVEUSER;

						  if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						  {
							  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int));

							  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						  }

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender);

						  MessageObject.vSetTransMessage();

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  break;

				      case ADDGROUP:

                          sprintf(MessageObject.stTransMessage.szSender,"Server");

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  MessageObject.stTransMessage.iMessageType = ADDGROUP;

						  if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						  {
							  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int)*2 + sizeof(szGroupName));

							  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						  }

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*4 + sizeof(MessageObject.stTransMessage.szSender) + sizeof(szGroupName);

						  MessageObject.vSetTransMessage();

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  break;

				      case REMOVEGROUP:

                          sprintf(MessageObject.stTransMessage.szSender,"Server");

						  memset(MessageObject.stTransMessage.szAddMessage,0,sizeof(MessageObject.stTransMessage.szAddMessage));

						  MessageObject.stTransMessage.iMessageType = REMOVEGROUP;

						  if(UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
						  {
							  memcpy(MessageObject.stTransMessage.szAddMessage,UserInfo.stSocketInfo[iSocketNo].szTempBuffer,sizeof(int)*2 + sizeof(szGroupName));

							  delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

							  UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
						  }

						  MessageObject.stTransMessage.iMessageLength = sizeof(int)*3 + sizeof(MessageObject.stTransMessage.szSender);

						  MessageObject.vSetTransMessage();

						  send(wParam,MessageObject.szMessageBuffer,MessageObject.stTransMessage.iMessageLength,0);

						  UserInfo.stSocketInfo[iSocketNo].iInfo = IDLE;
						  
						  if(WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket,hWnd,WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE))
						  {
							  vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");
							  
							  break;
						  }

						  break;
                  }

				  break;

			  case FD_CLOSE:
           
				  iUserID = 0;

				  iIndex = 0;

		          iNumberA = 0;

				  iIndex = UserInfo.iFindUserFromSocketNo(iSocketNo);

				  if (iIndex != EMPTYCONTENT)
				  {

					  while(iNumberA < MAX_USER)
					  {
						  iUserIDList[iNumberA] = EMPTYCONTENT;

                          iNumberA++;
					  }		          

				      iUserID = UserInfo.stUser[iIndex].iUserID;

		              iNumberA = 0;

		              while(iNumberA < MAX_USER)
					  {
						  if(UserInfo.stUser[iNumberA].iSocketNO != EMPTYCONTENT && UserInfo.stUser[iNumberA].iUserID != iUserID)
						  {
							  iUserIDList[iNumberA] = UserInfo.stUser[iNumberA].iUserID;
						  }

			              iNumberA++;
					  }
		   
		              iNumberA = 0;

		              while(lpLoopBuffer[iNumberA] != NULL)
					  {
						  iNumberA++;
					  }
				  
				      lpLoopBuffer[iNumberA] = new char[1024];

		              memcpy(lpLoopBuffer[iNumberA],&iUserID,sizeof(int));

		              memcpy(lpLoopBuffer[iNumberA] + sizeof(int),iUserIDList,sizeof(iUserIDList));

                      PostMessage(hMain_Window,WM_LOOP,REMOVEUSER,(LPARAM)lpLoopBuffer[iNumberA]);

		              vRemoveUser(iUserID);
					  
					  iNumberA = 0;

		              while(iNumberA < MAX_GROUP)
					  {
						  if(UserInfo.stGroup[iNumberA].iOwnerID == iUserID)
						  {
							  vRemoveGroup(UserInfo.stGroup[iNumberA].iGroupID);
						  }

						  iNumberA++;
					  }				      
				  }	
			  
                  UserInfo.vCleanSocket(iSocketNo);

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
	hText_ServerPort = CreateWindow(
		"static","Server Port",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		355,
		120,
		20,
		hWnd,(HMENU)IDC_Text_ServerPort,hInstance,NULL);

	hText_RegisterUser = CreateWindow(
		"static","Total User",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		5,
		120,
		20,
		hWnd,(HMENU)IDC_Text_RegisterUser,hInstance,NULL);

	hText_RegisterNumber = CreateWindow(
		"static","0",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		30,
		120,
		20,
		hWnd,(HMENU)IDC_Text_RegisterNumber,hInstance,NULL);

	hText_OnlineUser = CreateWindow(
		"static","Online User",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		55,
		120,
		20,
		hWnd,(HMENU)IDC_Text_OnlineUser,hInstance,NULL);

	hText_OnlineNumber = CreateWindow(
		"static","0",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		80,
		120,
		20,
		hWnd,(HMENU)IDC_Text_OnlineNumber,hInstance,NULL);

	hText_Group = CreateWindow(
		"static","Group Number",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		105,
		120,
		20,
		hWnd,(HMENU)IDC_Text_GroupNumber,hInstance,NULL);

	hText_GroupNumber = CreateWindow(
		"static","0",
		WS_CHILD | SS_CENTER | WS_VISIBLE,
		Windows_W - 135,
		130,
		120,
		20,
		hWnd,(HMENU)IDC_Text_GroupNumber,hInstance,NULL);


	
	hInput_ServerPort = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"EDIT","8000",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		Windows_W - 135,
		380,
		120,
		20,
		hWnd,(HMENU)IDC_Input_ServerPort,hInstance,NULL);

    hButton_StartUp = CreateWindow(
		"BUTTON",
		"Start Up",
		WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON ,
		Windows_W - 135,
		420,
		120,
		20,
		hWnd,(HMENU)IDC_Button_StartUp,hInstance,NULL);

	hOutput_Listbox = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"LISTBOX",
		NULL,
		WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | WS_HSCROLL,
		Windows_W - 505,
		5,
		365,
		440,
		hWnd,(HMENU)IDC_Output_Listbox,hInstance,NULL);
	

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

void vShowSocketError(int iSocketNo)
{
	char szErrno[128];

	memset(szErrno,0,128);

	switch(UserInfo.stSocketInfo[iSocketNo].iInfo)
	{
		 case LOGIN_SUCCESS:
		 case LOGIN_FAIL:
			 
			 sprintf(szErrno, "<Login reply %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;

		 case LOGOUT_SUCCESS:
		 case LOGOUT_FAIL:
			 
			 sprintf(szErrno, "<Logout reply %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;

         case RESGISTER_SUCCESS:
         case RESGISTER_FAIL:
			 
			 sprintf(szErrno, "<Register reply %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;
		 
		 case CREATEGROUP_SUCCESS:
		 case CREATEGROUP_FAIL:
			 
			 sprintf(szErrno, "<Create group reply %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;
		 
		 case DESTROYGROUP_SUCCESS:
		 case DESTROYGROUP_FAIL:
			 
			 sprintf(szErrno, "<Destroy group reply %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;
		 
		 case ADDUSER:
			 
			 sprintf(szErrno, "<Add user note %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;

		 case REMOVEUSER :
			 
			 sprintf(szErrno, "<Remove user note %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;

		 case ADDGROUP:
			 
			 sprintf(szErrno, "<Add group note %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;

		 case REMOVEGROUP:
			 
			 sprintf(szErrno, "<Remove group note %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;

		 case IDLE:
			 
			 sprintf(szErrno, "<Connecting %s>",ErrorObject.szWSAErrorMessage);
			 
			 break;
	}
				  
	vShowText(hOutput_Listbox, szErrno);
}

bool bGetPacket(int iSocketNo)
{
	char *szPacketTempBuffer = NULL;
	int	 iBytesReceived = 0;
	bool flag = false;

	szPacketTempBuffer = new char[MAX_BUFFER];

	iBytesReceived = recv(UserInfo.stSocketInfo[iSocketNo].socket,szPacketTempBuffer,MAX_BUFFER,0);

	if (iBytesReceived == SOCKET_ERROR)
	{

		ErrorObject.vGetWSAErrorMessage(WSAGetLastError());

		vShowText(hOutput_Listbox, ErrorObject.szWSAErrorMessage);


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
	struct stUserServer stUserServer;
	struct stGroupServer stGroupList[MAX_GROUP];
	struct stUserServer  stUserList[MAX_USER];
	struct sockaddr_in   stSockAddr;
    char szUserName[INFO_LENGTH];
	char szUserPassword[INFO_LENGTH];
	char szGroupName[INFO_LENGTH];
	char szGroupPassword[INFO_LENGTH];
	char szBuffer[512];
	bool bFlag;
	int iGroupID;
	int iUserID;
	int iIndex;
	int iNumberA;
	int iPort;
	int iPeerAddrLength;
	int iLine;
	int iUserIDList[MAX_USER];

	MessageObject.vGetTransMessage();

    switch(MessageObject.stTransMessage.iMessageType)
	{
	   case LOGIN_REQUEST:

		   iPort = 0;

		   iNumberA = 0;

		   iIndex = 0;

		   iLine = 0;

		   iPeerAddrLength = 0;

		   bFlag = false;

		   memset(szBuffer,0,sizeof(szBuffer));

		   memset(szUserName,0,sizeof(szUserName));

		   memset(szUserPassword,0,sizeof(szUserPassword));

		   memset(&stUserServer,0,sizeof(stUserServer));

		   memset(&stSockAddr,0,sizeof(sockaddr_in));

		   memset(stUserList,0,sizeof(stUserList));

		   memset(stGroupList,0,sizeof(stGroupList));

		   memcpy(&iPort,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   memcpy(szUserName,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],sizeof(szUserName));

		   memcpy(szUserPassword,&MessageObject.stTransMessage.szAddMessage[sizeof(int)+sizeof(szUserName)],sizeof(szUserPassword));

		   while(iNumberA < MAX_USER && !bFlag)
		   {
			   if(strcmp(UserInfo.stUser[iNumberA].szUserName,szUserName) != 0)
			   {
				   iNumberA++;
			   }
			   else
			   {
				   bFlag = true;
			   }			   
		   }

		   iIndex = iNumberA;

		   if (!bFlag)
		   {
			   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }
				   
			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [32];

			   sprintf(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,"User Name Not Found");

			   UserInfo.stSocketInfo[iSocketNo].iInfo = LOGIN_FAIL;

			   break;
		   }

		   if (bFlag && (strcmp(UserInfo.stUser[iIndex].szUserPassWord,szUserPassword) != 0))
		   {
			   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }
				   
			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [32];

			   sprintf(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,"Wrong Password");

			   UserInfo.stSocketInfo[iSocketNo].iInfo = LOGIN_FAIL;

			   break;
		   }

		   if (bFlag && (UserInfo.stUser[iIndex].iSocketNO != EMPTYCONTENT))
		   {
			   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }
				   
			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [32];

			   sprintf(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,"User already login");

			   UserInfo.stSocketInfo[iSocketNo].iInfo = LOGIN_FAIL;

			   break;
		   }

		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   if(iNumberA != iIndex && UserInfo.stUser[iNumberA].iSocketNO != EMPTYCONTENT)
			   {
				   stUserList[iNumberA].iUserID = UserInfo.stUser[iNumberA].iUserID;

			       sprintf(stUserList[iNumberA].szUserName,UserInfo.stUser[iNumberA].szUserName);

			       memcpy(&stUserList[iNumberA].Useraddr,&UserInfo.stUser[iNumberA].Useraddr,sizeof(struct sockaddr_in));
			   }

			   else
			   {
				   stUserList[iNumberA].iUserID = EMPTYCONTENT;
			   }
			   
			   iNumberA++;
		   }

		   iNumberA = 0;

		   while(iNumberA < MAX_GROUP)
		   {
			   stGroupList[iNumberA].iGroupID = UserInfo.stGroup[iNumberA].iGroupID;

			   sprintf(stGroupList[iNumberA].szGroupName,UserInfo.stGroup[iNumberA].szGroupName);

			   stGroupList[iNumberA].iOwnerID = UserInfo.stGroup[iNumberA].iOwnerID;

			   iNumberA++;
		   }

		   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
		   {
			   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
		   }

		   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [sizeof(int)+sizeof(stUserList)+sizeof(stGroupList)];

		   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&UserInfo.stUser[iIndex].iUserID,sizeof(int));

		   memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)],stUserList,sizeof(stUserList));

		   memcpy(&UserInfo.stSocketInfo[iSocketNo].szTempBuffer[sizeof(int)+sizeof(stUserList)],stGroupList,sizeof(stGroupList));

		   iPeerAddrLength = sizeof(struct sockaddr_in);

		   getpeername(UserInfo.stSocketInfo[iSocketNo].socket,(struct sockaddr*)&stSockAddr, &iPeerAddrLength);

		   UserInfo.stUser[iIndex].Useraddr.sin_port = htons(iPort);

		   UserInfo.stUser[iIndex].Useraddr.sin_family = AF_INET;

		   if (strcmp(inet_ntoa(stSockAddr.sin_addr),"127.0.0.1") == 0)
		   {
			   UserInfo.stUser[iIndex].Useraddr.sin_addr = GetHostIP();
		   }

		   else
		   {
			   UserInfo.stUser[iIndex].Useraddr.sin_addr = stSockAddr.sin_addr;
		   } 

		   UserInfo.stUser[iIndex].iSocketNO = iSocketNo;

		   stUserServer.iUserID = UserInfo.stUser[iIndex].iUserID;

		   sprintf(stUserServer.szUserName,UserInfo.stUser[iIndex].szUserName);

		   memcpy(&stUserServer.Useraddr,&UserInfo.stUser[iIndex].Useraddr,sizeof(struct sockaddr_in));
	   
 		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   iUserIDList[iNumberA] = EMPTYCONTENT;

               iNumberA++;
		   }

		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   if(UserInfo.stUser[iNumberA].iSocketNO != EMPTYCONTENT && UserInfo.stUser[iNumberA].iSocketNO != iSocketNo)
			   {
				   iUserIDList[iNumberA] = UserInfo.stUser[iNumberA].iUserID;
			   }

			   iNumberA++;
		   }

		   iNumberA = 0;

		   while(lpLoopBuffer[iNumberA] != NULL)
		   {
			   iNumberA++;
		   }

		   lpLoopBuffer[iNumberA] = new char[1024];

		   memcpy(lpLoopBuffer[iNumberA],&stUserServer,sizeof(stUserServer));

		   memcpy(lpLoopBuffer[iNumberA] + sizeof(stUserServer),iUserIDList,sizeof(iUserIDList));

           PostMessage(hMain_Window,WM_LOOP,ADDUSER,(LPARAM)lpLoopBuffer[iNumberA]);

		   SendMessage(hOutput_FListbox,LB_ADDSTRING,0,(LPARAM)UserInfo.stUser[iIndex].szUserName);

	       iLine = SendMessage(hOutput_FListbox,LB_GETCOUNT,0,0);

   	       UserInfo.stUser[iIndex].iIndex = iLine - 1;

           sprintf(szBuffer,"<User %s Login>",UserInfo.stUser[iIndex].szUserName);

           vShowText(hOutput_Listbox,szBuffer);

		   UserInfo.iOnlineUser++;

		   sprintf(szBuffer,"%d",UserInfo.iOnlineUser);

		   SetWindowText(hText_OnlineNumber,szBuffer);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = LOGIN_SUCCESS;
		   
		   break;

	   case LOGOUT_REQUEST:
           
           iUserID = 0;

		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   iUserIDList[iNumberA] = EMPTYCONTENT;

               iNumberA++;
		   }

		   memcpy(&iUserID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   if(UserInfo.stUser[iNumberA].iSocketNO != EMPTYCONTENT && UserInfo.stUser[iNumberA].iUserID != iUserID)
			   {
				   iUserIDList[iNumberA] = UserInfo.stUser[iNumberA].iUserID;
			   }

			   iNumberA++;
		   }
		   
		   iNumberA = 0;

		   while(lpLoopBuffer[iNumberA] != NULL)
		   {
			   iNumberA++;
		   }

		   lpLoopBuffer[iNumberA] = new char[1024];

		   memcpy(lpLoopBuffer[iNumberA],&iUserID,sizeof(int));

		   memcpy(lpLoopBuffer[iNumberA] + sizeof(int),iUserIDList,sizeof(iUserIDList));

           PostMessage(hMain_Window,WM_LOOP,REMOVEUSER,(LPARAM)lpLoopBuffer[iNumberA]);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = LOGOUT_SUCCESS;

		   break;

	   case RESGISTER_REQUEST:

		   iNumberA = 0;

		   iIndex = EMPTYCONTENT;

		   bFlag = false;

		   memset(szBuffer,0,sizeof(szBuffer));

		   memset(szUserName,0,sizeof(szUserName));

		   memset(szUserPassword,0,sizeof(szUserPassword));

		   memcpy(szUserName,MessageObject.stTransMessage.szAddMessage,sizeof(szUserName));

		   while(iNumberA < MAX_USER)
		   {
			   if(strcmp(UserInfo.stUser[iNumberA].szUserName,szUserName) == 0)
			   {
				   bFlag = true;
			   }

			   if(strlen(UserInfo.stUser[iNumberA].szUserName)== 0 && iIndex == EMPTYCONTENT)
			   {
				   iIndex = iNumberA;
			   } 

			   iNumberA++;
		   }

		   if(UserInfo.iRegisterUser >= MAX_USER)
		   {
			   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }
				   
			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [32];

			   sprintf(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,"User Number Is Full");

			   UserInfo.stSocketInfo[iSocketNo].iInfo = RESGISTER_FAIL;

			   break;
		   }

		   if(bFlag)
		   {
			   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }
				   
			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [32];

			   sprintf(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,"Used User Name");

			   UserInfo.stSocketInfo[iSocketNo].iInfo = RESGISTER_FAIL;

			   break;
		   }

		   memcpy(szUserPassword,&MessageObject.stTransMessage.szAddMessage[sizeof(szUserName)],sizeof(szUserPassword));

		   UserInfo.stUser[iIndex].iUserID = iIndex;

		   sprintf(UserInfo.stUser[iIndex].szUserName,szUserName);

		   sprintf(UserInfo.stUser[iIndex].szUserPassWord,szUserPassword);

		   UserInfo.stSocketInfo[iSocketNo].iInfo = RESGISTER_SUCCESS;

		   sprintf(szBuffer,"<User %s Register>",szUserName);
                          
		   vShowText(hOutput_Listbox,szBuffer);

		   UserInfo.iRegisterUser++;

		   sprintf(szBuffer,"%d",UserInfo.iRegisterUser);

		   SetWindowText(hText_RegisterNumber,szBuffer);

		   break;

	   case CREATEGROUP_REQUEST:
           
		   iUserID = 0;

		   memset(szUserName,0,sizeof(szGroupName));

		   memset(szUserPassword,0,sizeof(szGroupPassword));

		   memcpy(&iUserID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   memcpy(szGroupName,&MessageObject.stTransMessage.szAddMessage[sizeof(int)],sizeof(szGroupName));

		   memcpy(szGroupPassword,&MessageObject.stTransMessage.szAddMessage[sizeof(int)+sizeof(szGroupName)],sizeof(szGroupPassword));

		   iNumberA = 0;

		   iIndex = EMPTYCONTENT;

		   bFlag = false;

		   while(iNumberA < MAX_USER)
		   {
			   if(strcmp(UserInfo.stGroup[iNumberA].szGroupName,szGroupName) == 0)
			   {
				   bFlag = true;
			   }

			   if(strlen(UserInfo.stGroup[iNumberA].szGroupName) == 0 && iIndex == EMPTYCONTENT)
			   {
				   iIndex = iNumberA;
			   } 

			   iNumberA++;
		   }

           if(bFlag)
		   {
			   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }
				   
			   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [32];

			   sprintf(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,"Used Group Name");

			   UserInfo.stSocketInfo[iSocketNo].iInfo = CREATEGROUP_FAIL;

			   break;
		   }

		   iNumberA = 0;
		   
           SendMessage(hOutput_GListbox,LB_ADDSTRING,0,(LPARAM)szGroupName);

	       iNumberA = SendMessage(hOutput_GListbox,LB_GETCOUNT,0,0);

	       UserInfo.stGroup[iIndex].iIndex = iNumberA - 1;

		   UserInfo.stGroup[iIndex].iGroupID = iIndex;

		   UserInfo.stGroup[iIndex].iOwnerID = iUserID;

		   sprintf(UserInfo.stGroup[iIndex].szGroupName,szGroupName);

		   sprintf(UserInfo.stGroup[iIndex].szGroupPassWord,szGroupPassword);

		   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }
				   
		   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [sizeof(int)];

		   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&UserInfo.stGroup[iIndex].iGroupID,sizeof(int));
		   
		   UserInfo.stSocketInfo[iSocketNo].iInfo = CREATEGROUP_SUCCESS;


		   memset(szBuffer,0,sizeof(szBuffer));

		   sprintf(szBuffer,"<Group %s Created>",szGroupName);
                          
		   vShowText(hOutput_Listbox,szBuffer);

		   UserInfo.iGroupNumber++;

		   sprintf(szBuffer,"%d",UserInfo.iGroupNumber);

		   SetWindowText(hText_GroupNumber,szBuffer);
 
 		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   iUserIDList[iNumberA] = EMPTYCONTENT;

               iNumberA++;
		   }

		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   if(UserInfo.stUser[iNumberA].iSocketNO != EMPTYCONTENT && UserInfo.stUser[iNumberA].iSocketNO != iSocketNo)
			   {
				   iUserIDList[iNumberA] = UserInfo.stUser[iNumberA].iUserID;
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

		   memcpy(lpLoopBuffer[iNumberA] + sizeof(int),&UserInfo.stGroup[iIndex].iOwnerID,sizeof(int));

		   memcpy(lpLoopBuffer[iNumberA] + sizeof(int)*2,UserInfo.stGroup[iIndex].szGroupName,sizeof(szGroupName));

		   memcpy(lpLoopBuffer[iNumberA] + sizeof(int)*2 + sizeof(szGroupName),iUserIDList,sizeof(iUserIDList));

           PostMessage(hMain_Window,WM_LOOP,ADDGROUP,(LPARAM)lpLoopBuffer[iNumberA]);

		   break;

	   case DESTROYGROUP_REQUEST:

		   iGroupID = 0;

		   memcpy(&iGroupID,MessageObject.stTransMessage.szAddMessage,sizeof(int));

		   iIndex = EMPTYCONTENT;

		   iIndex = UserInfo.iFindGroupFromGroupID(iGroupID);
		   
 		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   iUserIDList[iNumberA] = EMPTYCONTENT;

               iNumberA++;
		   }

		   iNumberA = 0;

		   while(iNumberA < MAX_USER)
		   {
			   if(UserInfo.stUser[iNumberA].iSocketNO != EMPTYCONTENT && UserInfo.stUser[iNumberA].iSocketNO != iSocketNo)
			   {
				   iUserIDList[iNumberA] = UserInfo.stUser[iNumberA].iUserID;
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

		   memcpy(lpLoopBuffer[iNumberA] + sizeof(int),iUserIDList,sizeof(iUserIDList));

           PostMessage(hMain_Window,WM_LOOP,REMOVEGROUP,(LPARAM)lpLoopBuffer[iNumberA]);

		   if (UserInfo.stSocketInfo[iSocketNo].szTempBuffer != NULL)
			   {
				   delete [] UserInfo.stSocketInfo[iSocketNo].szTempBuffer;

				   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = NULL;
			   }
				   
		   UserInfo.stSocketInfo[iSocketNo].szTempBuffer = new char [sizeof(int)];

		   memcpy(UserInfo.stSocketInfo[iSocketNo].szTempBuffer,&iGroupID,sizeof(int));
		   
		   UserInfo.stSocketInfo[iSocketNo].iInfo = DESTROYGROUP_SUCCESS;

		   vRemoveGroup(iGroupID);

		   break;
	}
}

void vRemoveUser(int iUserID)
{
	int iNumberA;
	int iNumberB;
	char szBuffer[64];

    iNumberA = 0;

	iNumberB = 0;
	
	memset(szBuffer,0,sizeof(szBuffer));

	iNumberA = UserInfo.iFindUserFromUserID(iUserID);

	if (iNumberA != EMPTYCONTENT)
	{
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

        sprintf(szBuffer,"<User %s Logout>",UserInfo.stUser[iNumberA].szUserName);

		vShowText(hOutput_Listbox,szBuffer);

		memset(&UserInfo.stUser[iNumberA].Useraddr,0,sizeof(sockaddr_in));

		UserInfo.iOnlineUser--;

		sprintf(szBuffer,"%d",UserInfo.iOnlineUser);

		SetWindowText(hText_OnlineNumber,szBuffer);
	}
}


void vRemoveGroup(int iGroupIndex)
{
	int iNumberA;
	char szBuffer[256];

	iNumberA = 0;

	if (iGroupIndex != EMPTYCONTENT)
	{
		SendMessage(hOutput_GListbox,LB_DELETESTRING,UserInfo.stGroup[iGroupIndex].iIndex,0);

		while(iNumberA<MAX_GROUP)
		{
			if (UserInfo.stGroup[iNumberA].iIndex > UserInfo.stGroup[iGroupIndex].iIndex)
			{
				UserInfo.stGroup[iNumberA].iIndex = UserInfo.stGroup[iNumberA].iIndex - 1;
			}

			iNumberA++;
		}

				
		memset(szBuffer,0,sizeof(szBuffer));

		sprintf(szBuffer,"<Group %s Destroyed>",UserInfo.stGroup[iGroupIndex].szGroupName);
                          
		vShowText(hOutput_Listbox,szBuffer);

		UserInfo.iGroupNumber--;

		sprintf(szBuffer,"%d",UserInfo.iGroupNumber);

		SetWindowText(hText_GroupNumber,szBuffer);


		UserInfo.stGroup[iGroupIndex].iIndex = EMPTYCONTENT;

		UserInfo.stGroup[iGroupIndex].iGroupID = EMPTYCONTENT;

		UserInfo.stGroup[iGroupIndex].iOwnerID = EMPTYCONTENT;

		sprintf(UserInfo.stGroup[iGroupIndex].szGroupName,"");

		sprintf(UserInfo.stGroup[iGroupIndex].szGroupPassWord,"");
	}
}

bool bServerStartUp()
{
	int     iPort;
	int     iSocketNo;
    int     iNumberA;
	char    szBuffer[INFO_LENGTH];
	struct  sockaddr_in stAddr;

	iPort = 0;

	iSocketNo = 0;

	memset(&stAddr,0,sizeof(struct  sockaddr_in));

	memset(szBuffer,0,sizeof(szBuffer));

	iSocketNo = UserInfo.iGetSocket();

	UserInfo.stSocketInfo[iSocketNo].socket = socket(AF_INET, SOCK_STREAM, 0);

	GetWindowText(hInput_ServerPort,szBuffer,INFO_LENGTH);

	iPort = atoi(szBuffer);

    stAddr.sin_family      = AF_INET;

	stAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	stAddr.sin_port        = htons(iPort);

	if(bind(UserInfo.stSocketInfo[iSocketNo].socket,(struct sockaddr*)&stAddr,sizeof(struct  sockaddr_in)) != 0)
	{
		closesocket(UserInfo.stSocketInfo[iSocketNo].socket);

		return false;
	}

	if (WSAAsyncSelect(UserInfo.stSocketInfo[iSocketNo].socket, hMain_Window, WM_SOCKET, FD_ACCEPT | FD_CLOSE))
	{
		vShowText(hOutput_Listbox, "<WSAAsyncSelect fail>");

	    return false;
	}

	listen(UserInfo.stSocketInfo[iSocketNo].socket,5);

    UserInfo.stSocketInfo[iSocketNo].iInfo = LISTENSOCKET;
	
	SetWindowText(hButton_StartUp,"Shutdown");

	SendMessage(hInput_ServerPort,EM_SETREADONLY,TRUE,NULL);

	UserInfo.bServerOnLine = true;

	iNumberA = 0;

	while(iNumberA < MAX_USER)
	{
		if (lpLoopBuffer[iNumberA] != NULL)
		{
			delete [] lpLoopBuffer[iNumberA];			
		}

		lpLoopBuffer[iNumberA] = NULL;

		iNumberA++;
	}

	return true;
}

void vServerShutdown()
{
	int iNumberA;

	iNumberA = 0;

	while(iNumberA < MAX_SOCKET)
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
	
	UserInfo.vInitGroupList();

	UserInfo.vInitUserList();

	UserInfo.bServerOnLine = false;

	UserInfo.iRegisterUser = 0;

	UserInfo.iOnlineUser = 0;

	UserInfo.iGroupNumber = 0;

	SetWindowText(hText_RegisterNumber,"0");

	SetWindowText(hText_OnlineNumber,"0");

	SetWindowText(hText_GroupNumber,"0");

	SetWindowText(hButton_StartUp,"Start Up");

    SendMessage(hInput_ServerPort,EM_SETREADONLY,FALSE,NULL);

	SendMessage(hOutput_FListbox,LB_RESETCONTENT,0,0);

	SendMessage(hOutput_GListbox,LB_RESETCONTENT,0,0);
}

struct in_addr GetHostIP()
{
  char       szHostName[128]; 
  unsigned   long   ulIP;
  struct   hostent *  stHostent;  
  struct   in_addr    stAddr;

     
  gethostname(szHostName,128);
  
  stHostent = gethostbyname(szHostName);

  if(*(DWORD*)stHostent->h_addr_list[1] !=NULL)
  {
	  ulIP = *(DWORD*)stHostent->h_addr_list[1];
  }

  else
  {
	  ulIP = *(DWORD*)stHostent->h_addr_list[0];
  }   
		  
  stAddr.s_addr = ulIP;  
	  
  return(stAddr); 
}
