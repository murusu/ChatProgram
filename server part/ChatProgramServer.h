#ifndef MAIN_WINDOW

#define MAIN_WINDOW

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <basetsd.h>

LPCTSTR		lpszCLASSName     = "ChatProgramServer";
LPCTSTR		lpszWINDOWSTitle  = "ChatProgramServer";

#define Windows_H                   480
#define Windows_W                   640



#define	IDC_Input_ServerPort		40001

#define	IDC_Text_ServerPort		    40002
#define	IDC_Text_RegisterUser       40003    
#define	IDC_Text_RegisterNumber     40004
#define	IDC_Text_OnlineUser         40005
#define	IDC_Text_OnlineNumber       40006
#define	IDC_Text_GroupNumber        40007 
#define	IDC_Text_Group              40008

#define	IDC_Output_Listbox			40009
#define	IDC_Output_FListbox	        40010
#define	IDC_Output_GListbox	        40011

#define	IDC_Button_StartUp          40012



#define WM_SOCKET WM_USER+1

#define WM_LOOP   WM_USER+2




HWND        hMain_Window        = NULL;

HWND		hText_ServerPort	= NULL;
HWND		hText_RegisterUser  = NULL;
HWND		hText_RegisterNumber= NULL;
HWND		hText_OnlineUser    = NULL;
HWND		hText_OnlineNumber  = NULL;
HWND		hText_GroupNumber   = NULL;
HWND		hText_Group         = NULL;

HWND		hOutput_Listbox		= NULL;
HWND		hOutput_FListbox    = NULL;
HWND		hOutput_GListbox    = NULL;

HWND		hInput_ServerPort	= NULL;

HWND		hButton_StartUp 	= NULL;


LRESULT CALLBACK fnMessageProcessor (HWND, UINT, WPARAM, LPARAM);


void vCreateInterface(HWND hWnd,HINSTANCE hInstance);
bool bInitProgram(HWND hWnd);
void vShowText(HWND	hWnd, char *szText);


bool bServerStartUp(void);
void vServerShutdown(void);

void vInitSocketInfo(void);
int  iGetSocket(void);
void vMessageproc(char *Message);
void vShowSocketError(int iSocketNo);


bool bGetPacket(int iSocketNo);

void vProcessReadMessage(int iSocketNo);
void vRemoveUser(int iUserID);
void vRemoveGroup(int iGroupID);



struct in_addr GetHostIP(void);
#endif




