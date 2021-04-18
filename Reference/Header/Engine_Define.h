#ifndef __ENGINE_DEFINE_H__
#define __ENGINE_DEFINE_H__

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <io.h>
#include <process.h>


// STL Headers
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>

// Thread Headers
#include <process.h>

// DirectX Headers
#include <d3d9.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0X0800
#include <dinput.h>

// Memory Leak Check
#ifdef _DEBUG
// Disable Warning Num: 4005
#pragma warning (disable : 4005)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif 

#endif

// DLL 프로젝트에서 템플릿 사용시 발생하는 C4251 경고 무시
#pragma warning (disable : 4251)

// FMOD
#include "fmod.h"
#pragma comment(lib, "fmodex_vc.lib")

// Engine Headers
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Functor.h"
#include "Engine_Struct.h"

// using namespace
USING(Engine)
USING(std)

#endif // !__ENGINE_DEFINE_H__
