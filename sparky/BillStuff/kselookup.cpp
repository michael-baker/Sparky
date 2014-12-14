/*  ------------------------------------------------------------------------
	  kselookup.cpp by ruiner
	    - updated v1.36 - 01/14/03
		- added v1.35 - 01/14/03
	------------------------------------------------------------------------ */

#include "..\client.h"
#include "hook.h"
#include "kselookup.h"
#include "winsock.h"
#include "help.h"
#pragma comment(lib,"wsock32.lib")

#define MAX_PAGE    (1024*64)

/*Global*/
CKSELookup gKSELookup;

DECLARE_COMMAND_2(gKSELookup, KSELookup);

int GetAddr(const char* HostName, int Port, struct sockaddr* Result)
{
	struct hostent*     Host;
	SOCKADDR_IN         Address;

	memset(Result, 0, sizeof(*Result));
	memset(&Address, 0, sizeof(Address));
	Host                = gethostbyname(HostName);
	if(Host != NULL)
	{
		Address.sin_family  = AF_INET;
		Address.sin_port    = htons((short)Port);
		memcpy(&Address.sin_addr, Host->h_addr_list[0], Host->h_length);
		memcpy(Result, &Address, sizeof(Address));
	}
	return Host != NULL;
} 

void CKSELookup::Post_HUD_Init(void)
{
	HOOK_COMMAND_2("bkselookup", KSELookup);
	gHelp.addCommand("bkselookup", "Checks a wonid against the kse tracker database and retrieves the last five names used.\nUsage: bkselookup <wonid>");
}

void CKSELookup::Cmd_KSELookup(void)
{
	char page[255];
	
	if((gEngfuncs.Cmd_Argc() < 2) || (!atol(gEngfuncs.Cmd_Argv(1))))
	{
		gEngfuncs.pfnConsolePrint("[sparky] invalid bkselookup syntax. usage: bkselookup <wonid>\n");
		return;
	}

	sprintf(page, "http://www.konspiracy.org/tracker/trackerbot.php?wonid=%s\n", gEngfuncs.Cmd_Argv(1));

    WSADATA	WsaData;
    struct sockaddr     SockAddr;
    SOCKET              Socket;
	char				Request[1024];
	char				PageBuffer[MAX_PAGE];
	char*				pBuffer = PageBuffer;
	char				buf[256];
	int					nret;
	int					Read;

    WSAStartup(0x0001, &WsaData);

	if(GetAddr("www.konspiracy.org", 80, &SockAddr))
        {

        Socket = socket(AF_INET, SOCK_STREAM, 0);
        nret = connect(Socket, &SockAddr, sizeof(SockAddr));
        if(nret>=0)
		{
            sprintf(Request, "GET http://www.konspiracy.org/tracker/trackerbot.php?wonid=%s HTTP/1.0\n\n", gEngfuncs.Cmd_Argv(1));
			gEngfuncs.pfnConsolePrint("[sparky] sending request to tracker.konspiracy.org, please wait...\n");
            nret = send(Socket, Request, strlen(Request), 0);
			if (nret == SOCKET_ERROR)
				gEngfuncs.pfnConsolePrint("[sparky] socket error getting information from tracker.konspiracy.org\n");
			else {
				for (Read=0; Read < MAX_PAGE;)
				{
					int nBytesRead;
					nBytesRead = recv(Socket, pBuffer, MAX_PAGE-Read, 0);
					if (nBytesRead == SOCKET_ERROR || nBytesRead == 0)
						break;
					else {
						Read += nBytesRead;
						pBuffer += nBytesRead;
					}
				}
				
				strcpy(buf, "\0");
				for(int count=130; count < int(strlen(PageBuffer)); count++)
					if(PageBuffer[count]>31)
						sprintf(buf,"%s%c", buf, PageBuffer[count]);
				sprintf(buf,"%s\n", buf);
				gEngfuncs.pfnConsolePrint(buf);
				closesocket(Socket);
			}
        } else {
			gEngfuncs.pfnConsolePrint("[sparky] error(2) connecting to tracker.konspiracy.org\n");
		}
	} else {
		gEngfuncs.pfnConsolePrint("[sparky] error(1) connecting to tracker.konspiracy.org\n");
	}
	WSACleanup();
}
