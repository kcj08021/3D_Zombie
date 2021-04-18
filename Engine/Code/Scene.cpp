#include "..\Header\Scene.h"

// ������
CScene::CScene(LPDIRECT3DDEVICE9 _pGraphicDevice): m_pGraphicDevice(_pGraphicDevice){
	SafeAddRef(_pGraphicDevice);
}

// �Ҹ���
void CScene::Free(){
	SafeRelease(m_pGraphicDevice);
}