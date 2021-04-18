#include "..\Header\Scene.h"

// 持失切
CScene::CScene(LPDIRECT3DDEVICE9 _pGraphicDevice): m_pGraphicDevice(_pGraphicDevice){
	SafeAddRef(_pGraphicDevice);
}

// 社瑚切
void CScene::Free(){
	SafeRelease(m_pGraphicDevice);
}