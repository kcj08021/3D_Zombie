#include "..\Header\Texture.h"



// 생성자
CTexture::CTexture(LPDIRECT3DDEVICE9 _pGraphicDevice):CComponent(_pGraphicDevice), m_uiTextureCount(0){
}

// 복사 생성자
CTexture::CTexture(const CTexture & _Texture):CComponent(_Texture), m_uiTextureCount(_Texture.m_uiTextureCount), m_TextureVec(_Texture.m_TextureVec){
	// 벡터간 인자를 복사해줄때 얕은 복사이므로 각 원소들의 레퍼런스 카운트 증가
	for(auto& elem : _Texture.m_TextureVec)
		SafeAddRef(elem);
}

// Initialize with Prototype
HRESULT CTexture::InitComponent_Proto(TEXTURE_TYPE _eType, const _tchar* _szFilePath, _uint _uiTextureCount){
	_tchar szFilePath[MAX_PATH] = L"";

	// 이미지 갯수 만큼 벡터크기 예약
	m_uiTextureCount = _uiTextureCount;
	m_TextureVec.reserve(_uiTextureCount);

	// 텍스쳐 생성
	for(size_t i = 0; i < _uiTextureCount; ++i){
		LPDIRECT3DBASETEXTURE9 pTexture = nullptr;
		HRESULT hr = 0;

		wsprintf(szFilePath, _szFilePath, i);

		switch(_eType){
		case TEXTURE_GENERAL:	// 일반적인 텍스쳐
			hr = D3DXCreateTextureFromFile(CComponent::GetGraphicDevice(), szFilePath, reinterpret_cast<LPDIRECT3DTEXTURE9*>(&pTexture));
			break;
			
		case TEXTURE_CUBE:		// 큐브 텍스쳐
			hr = D3DXCreateCubeTextureFromFile(CComponent::GetGraphicDevice(), szFilePath, reinterpret_cast<LPDIRECT3DCUBETEXTURE9*>(&pTexture));
			break;
		default:
			break;
		}

		if(FAILED(hr))
			return E_FAIL;

		// 컨테이너에 보관
		m_TextureVec.push_back(pTexture);
	}

	return NOERROR;
}

// Initialize with Clone
HRESULT CTexture::InitComponent_Clone(void* _pArgument){
	return NOERROR;
}

// Free
void CTexture::Free(){
	for(auto& elem : m_TextureVec)
		SafeRelease(elem);
	m_TextureVec.clear();

	CComponent::Free();
}

// 복제 생성 함수
CComponent * CTexture::CreateClone(void* _pArgument){
	CComponent* pInstance = new CTexture(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		MSG_BOX(L"CTexture Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// 프로토타입 생성 함수
CTexture * CTexture::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, TEXTURE_TYPE _eType, const _tchar* _szFilePath, _uint _uiTextureCount/* = 1*/){
	CTexture* pInstance = new CTexture(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_eType, _szFilePath, _uiTextureCount))){
		MSG_BOX(L"CTexture Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}
