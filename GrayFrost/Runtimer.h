#include "MSCorEE.h"
#include <sys\types.h>
#include <sys\stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include "metahost.h"
#import "mscorlib.tlb" raw_interfaces_only

using namespace std;


using namespace mscorlib;

// Import mscorlib typelib. Using 1.0 for maximum backwards compatibility
#pragma comment(lib, "mscoree.lib")

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call,  LPVOID lpReserved);

#define CLR_V10 L"v1.0.3705"
#define CLR_V11 L"v1.1.4322"
#define CLR_V20 L"v2.0.50727"
#define CLR_V30 L"v3.0"
#define CLR_V35 L"v3.5"
#define CLR_V40 L"v4.0.30319"
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

bool Bootstrap();

//#endif