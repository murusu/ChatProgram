#ifndef MAIN_WINDOW

#define MAIN_WINDOW

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <basetsd.h>

LPCTSTR		lpszCLASSName     = "ChatProgramClient";
LPCTSTR		lpszWINDOWSTitle  = "ChatProgramClient";

#define Windows_H                   480
#define Windows_W                   640



#define	IDC_Input_ServerIP			40001
#define	IDC_Input_ServerPort		40002
#define	IDC_Input_UserName			40003
#define	IDC_Input_UserPassword  	40004
#define	IDC_Input_GroupName         40005
#define	IDC_Input_GroupPassword     40006
#define	IDC_Input_Editbox			40007

#define	IDC_Text_ServerIP		    40008
#define	IDC_Text_ServerPort		    40009
#define	IDC_Text_UserName			40010
#define	IDC_Text_UserPassword       40011
#define	IDC_Text_TalkTo	            40012
#define	IDC_Text_GroupName          40013
#define	IDC_Text_GroupPassword      40014

#define	IDC_Output_Listbox			40015
#define	IDC_Output_FListbox	        40016
#define	IDC_Output_GListbox	        40017

#define	IDC_Button_Login			40018
#define	IDC_Button_Register		    40019
#define	IDC_Button_CreateGroup      40020
#define	IDC_Button_Send  		    40021
#define	IDC_Button_JoinGroup        40022


#define WM_SOCKET WM_USER+1

#define WM_LOOP   WM_USER+2


HWND        hMain_Window        = NULL;

HWND		hText_ServerIP	    = NULL;
HWND		hText_ServerPort	= NULL;
HWND		hText_UserName		= NULL;
HWND		hText_UserPassword  = NULL;
HWND		hText_TalkTo        = NULL;	
HWND		hText_GroupName     = NULL;
HWND		hText_GroupPassword = NULL;

HWND		hOutput_Listbox		= NULL;
HWND		hOutput_FListbox    = NULL;
HWND		hOutput_GListbox    = NULL;

HWND		hInput_ServerIP	    = NULL;
HWND		hInput_ServerPort	= NULL;
HWND		hInput_UserName		= NULL;
HWND		hInput_UserPassword = NULL;
HWND		hInput_Editbox		= NULL;
HWND		hInput_GroupName    = NULL;
HWND		hInput_GroupPassword= NULL;

HWND		hButton_Login		= NULL;
HWND		hButton_Register	= NULL;
HWND		hButton_CreateGroup = NULL;
HWND		hButton_JoinGroup   = NULL;
HWND		hButton_Send    	= NULL;


LRESULT CALLBACK fnMessageProcessor (HWND, UINT, WPARAM, LPARAM);


void vCreateInterface(HWND hWnd,HINSTANCE hInstance);
bool bInitProgram(HWND hWnd);
void vShowText(HWND	hWnd, char *szText);

void vInitSocketInfo(void);
int  iGetSocket(void);
void vMessageproc(char *Message);
void vLockServerInput(bool flag);
void vLockGroupInput(bool flag);
int  iFindGroupFromListIndex(int iIndex);
void vShowSocketError(int iSocketNo);
void vUserLogout(void);

bool bSendToUser(int iIndex, char * szSendMessage, int iGroupID);
bool bGetPacket(int iSocketNo);

void vProcessReadMessage(int iSocketNo);
void vAddUser(struct stUser stAddUser);
void vRemoveUser(int iUserID);
void vAddGroup(struct stGroup stAddGroup);
void vRemoveGroup(int iGroupID);



void vShowGroupMember(int iGroupID);
void vShowUser(void);
#endif




