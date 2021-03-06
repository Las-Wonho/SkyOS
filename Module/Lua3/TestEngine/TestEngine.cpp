// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h> 
#include <windows.h> 
#include "SkyMockInterface.h"
#include "I_LuaModule.h"

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void (*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_LuaModule*(*PGetLuaModule)();


void* GetModuleFunction(HINSTANCE handle, const char* func_name)
{
	return (void*)GetProcAddress(handle, func_name);
}

int main()
{
	HINSTANCE dllHandle = NULL;
	
	//디버그엔진 모듈을 로드한다.
	dllHandle = LoadLibrary("Lua3.dll");
	
	//디버그엔진 모듈이 익스포트하는 SetSkyMockInterface 함수와 GetDebugEngineDLL 함수를 얻어낸다.
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetLuaModule GetLuaModuleInterface = (PGetLuaModule)GetModuleFunction(dllHandle, "GetLuaModule");

	if (!GetLuaModuleInterface || !SetSkyMockInterface)
	{
		printf("GetLuaModuleInterface Aquired failed!\n");
		return 0;
	}

	//SetSkyMockInterface 함수를 사용해서 디버그엔진 모듈에 파일인터페이스와 입출력, 화면출력 인터페이스를 제공한다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
		
	I_LuaModule* pLuaModule = GetLuaModuleInterface();

	if(pLuaModule == nullptr)
	{
		printf("Lua Module Creation Fail!\n");
		return 0;
	}
	char filename[10] = "1.lua";

	pLuaModule->InitLua();
	pLuaModule->DoFile(filename);
	pLuaModule->CloseLua();

	return 0;
}