#include "SDK/Interface/NFPlatform.h"

#ifdef NF_DEBUG_MODE

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "NFCore_d.lib" )
#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
////#pragma comment( lib, "RakNet_LibStatic_Debug_x64.a" )
#pragma comment( lib, "NFNet_d.a" )
#pragma comment( lib, "NFCore_d.a" )
#pragma comment( lib, "NFMessageDefine_d.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "NFNet.lib" )
#pragma comment( lib, "NFCore.lib" )
#pragma comment( lib, "NFMessageDefine.lib" )
#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
//#pragma comment( lib, "RakNet_LibStatic_Release_x64.a" )
#pragma comment( lib, "NFNet.a" )
#pragma comment( lib, "NFCore.a" )
#pragma comment( lib, "NFMessageDefine.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif
