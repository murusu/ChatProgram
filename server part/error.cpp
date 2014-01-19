#include "error.h"


void ErrorObject::vGetWSAErrorMessage(int err)
{
	char error[64];

	switch(err)
	{
	    case 6:
         sprintf(error,"Specified event object handle is invalid");
		break;

		case 8:
         sprintf(error,"Insufficient memory available");
		break;

		case 87:
         sprintf(error,"One or more parameters are invalid");
		break;

		case 995:
         sprintf(error,"Overlapped operation aborted");
		break;

		case 996:
         sprintf(error,"Overlapped I/O event object not in signaled state");
		break;

		case 997:
         sprintf(error,"Overlapped operations will complete later");
		break;

		case 10004:
         sprintf(error,"Interrupted function call");
		break;

		case 10009:
         sprintf(error,"File handle is not valid");
		break;

		case 10013:
         sprintf(error,"Permission denied");
		break;

		case 10014:
         sprintf(error,"Bad address");
		break;

		case 10022:
         sprintf(error,"Invalid argument");
		break;

		case 10024:
         sprintf(error,"Too many open files");
		break;

		case 10035:
         sprintf(error,"Resource temporarily unavailable");
		break;

		case 10036:
         sprintf(error,"Operation now in progress");
		break;

		case 10037:
         sprintf(error,"Operation already in progress");
		break;

		case 10038:
         sprintf(error,"Socket operation on nonsocket");
		break;

		case 10039:
         sprintf(error,"Destination address required");
		break;

		case 10040:
         sprintf(error,"Message too long");
		break;

		case 10041:
         sprintf(error,"Protocol wrong type for socket");
		break;

		case 10042:
         sprintf(error,"Bad protocol option");
		break;

		case 10043:
         sprintf(error,"Protocol not supported");
		break;

		case 10044:
         sprintf(error,"Socket type not supported");
		break;

		case 10045:
         sprintf(error,"Operation not supported");
		break;

		case 10046:
         sprintf(error,"Protocol family not supported");
		break;

		case 10047:
         sprintf(error,"Address family not supported by protocol family");
		break;

		case 10048:
         sprintf(error,"Address already in use");
		break;

		case 10049:
         sprintf(error,"Cannot assign requested address");
		break;

		case 10050:
         sprintf(error,"Network is down");
		break;

		case 10051:
         sprintf(error,"Network is unreachable");
		break;

		case 10052:
         sprintf(error,"Network dropped connection on reset");
		break;

		case 10053:
         sprintf(error,"Software caused connection abort");
		break;

		case 10054:
         sprintf(error,"Connection reset by peer");
		break;

		case 10055:
         sprintf(error,"No buffer space available");
		break;

		case 10056:
         sprintf(error,"Socket is already connected");
		break;

		case 10057:
         sprintf(error,"Socket is not connected");
		break;

		case 10058:
         sprintf(error,"Cannot send after socket shutdown.");
		break;

		case 10059:
         sprintf(error,"Too many references");
		break;

		case 10060:
         sprintf(error,"Connection timed out");
		break;

		case 10061:
         sprintf(error,"Connection refused");
		break;

		case 10062:
         sprintf(error,"Cannot translate name");
		break;

		case 10063:
         sprintf(error,"Name too long");
		break;

		case 10064:
         sprintf(error,"Host is down");
		break;

		case 10065:
         sprintf(error,"No route to host");
		break;

		case 10066:
         sprintf(error,"Directory not empty");
		break;

		case 10067:
         sprintf(error,"Too many processes");
		break;

		case 10068:
         sprintf(error,"User quota exceeded");
		break;

		case 10069:
         sprintf(error,"Disk quota exceeded");
		break;

		case 10070:
         sprintf(error,"Stale file handle reference");
		break;

		case 10071:
         sprintf(error,"Item is remote");
		break;

		case 10091:
         sprintf(error,"Network subsystem is unavailable");
		break;

		case 10092:
         sprintf(error,"Winsock.dll version out of range");
		break;

		case 10093:
         sprintf(error,"Successful WSAStartup not yet performed");
		break;

		case 10101:
         sprintf(error,"Graceful shutdown in progress");
		break;

		case 10102:
         sprintf(error,"No more results");
		break;

		case 10103:
         sprintf(error,"Call has been canceled");
		break;

		case 10104:
         sprintf(error,"Procedure call table is invalid");
		break;

		case 10105:
         sprintf(error,"Service provider is invalid");
		break;

		case 10106:
         sprintf(error,"Service provider failed to initialize");
		break;

		case 10107:
         sprintf(error,"System call failure");
		break;

		case 10108:
         sprintf(error,"Service not found");
		break;

		case 10109:
         sprintf(error,"Class type not found");
		break;

		case 10110:
         sprintf(error,"No more results");
		break;

		case 10111:
         sprintf(error,"Call was canceled");
		break;

		case 10112:
         sprintf(error,"Database query was refused");
		break;

		case 11001:
         sprintf(error,"Host not found");
		break;

		case 11002:
         sprintf(error,"Nonauthoritative host not found");
		break;

		case 11003:
         sprintf(error,"This is a nonrecoverable error");
		break;

		case 11004:
         sprintf(error,"Valid name, no data record of requested type");
		break;

		case 11005:
         sprintf(error,"QOS receivers");
		break;

		case 11006:
         sprintf(error,"QOS senders");
		break;

		case 11007:
         sprintf(error,"No QOS senders");
		break;

		case 11008:
         sprintf(error,"QOS no receivers");
		break;

		case 11009:
         sprintf(error,"QOS request confirmed");
		break;

		case 11010:
         sprintf(error,"QOS admission error");
		break;

		case 11011:
         sprintf(error,"QOS policy failure");
		break;

		case 11012:
         sprintf(error,"QOS bad style");
		break;

		case 11013:
         sprintf(error,"QOS bad object");
		break;

		case 11014:
         sprintf(error,"QOS traffic control error");
		break;

		case 11015:
         sprintf(error,"QOS generic error");
		break;

		case 11016:
         sprintf(error,"QOS service type error");
		break;

		case 11017:
         sprintf(error,"QOS flowspec error");
		break;

		case 11018:
         sprintf(error,"Invalid QOS provider buffer");
		break;

		case 11019:
         sprintf(error,"Invalid QOS filter style.");
		break;

		case 11020:
         sprintf(error,"Invalid QOS filter type");
		break;

		case 11021:
         sprintf(error,"Incorrect QOS filter count");
		break;

		case 11022:
         sprintf(error,"Invalid QOS object length");
		break;

		case 11023:
         sprintf(error,"Incorrect QOS flow count");
		break;

		case 11024:
         sprintf(error,"Unrecognized QOS object");
		break;

		case 11025:
         sprintf(error,"Invalid QOS policy object");
		break;

		case 11026:
         sprintf(error,"Invalid QOS flow descriptor");
		break;

		case 11027:
         sprintf(error,"Invalid QOS provider-specific flowspec");
		break;

		case 11028:
         sprintf(error,"Invalid QOS provider-specific filterspec");
		break;

		case 11029:
         sprintf(error,"Invalid QOS shape discard mode object");
		break;

		case 11030:
         sprintf(error,"Invalid QOS shaping rate object");
		break;

		case 11031:
         sprintf(error,"Reserved policy QOS element type");
		break;

		default:
		 sprintf(error,"Unkown error");

	}

	sprintf(szWSAErrorMessage,"error %d: %s", err,error);
}
