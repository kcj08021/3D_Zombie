#ifndef __ENGINE_DEFINE_H__
#define __ENGINE_DEFINE_H__

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
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

// DLL ������Ʈ���� ���ø� ���� �߻��ϴ� C4251 ��� ����
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
