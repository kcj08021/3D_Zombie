#ifndef __CLIENT_HEADERS_H__
#define __CLIENT_HEADERS_H__

#include "stdafx.h"
#include "Client_Macro.h"
#include "Client_Struct.h"
#include "Client_Defines.h"

// Window Size
constexpr unsigned int WINCX = 1280;
constexpr unsigned int WINCY = 720;

// Extern
extern HINSTANCE g_hInst;
extern HWND g_hWnd;

// Scene Type
enum SCENE_TYPE{ STATIC_SCENE = 0, TITLE_SCENE, STAGE_SCENE, LOADING_SCENE, SCENE_END};
/* 0번은 반드시 STATIC_SCENE으로 할 것 */

#endif // !__CLIENT_HEADERS_H__


