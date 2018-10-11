#include "includes.h"
#include "version.h"







const char* checkcsgotype()
{
	if (strstr(GetCommandLineA(), "-scaleform"))
		return "client.dll";
	else
		return "client_panorama.dll";
}