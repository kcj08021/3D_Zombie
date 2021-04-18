#include "..\Header\Texture.h"



// ������
CTexture::CTexture(LPDIRECT3DDEVICE9 _pGraphicDevice):CComponent(_pGraphicDevice), m_uiTextureCount(0){
}

// ���� ������
CTexture::CTexture(const CTexture & _Texture):CComponent(_Texture), m_uiTextureCount(_Texture.m_uiTextureCount), m_TextureVec(_Texture.m_TextureVec){
	// ���Ͱ� ���ڸ� �������ٶ� ���� �����̹Ƿ� �� ���ҵ��� ���۷��� ī��Ʈ ����
	for(auto& elem : _Texture.m_TextureVec)
		SafeAddRef(elem);
}

// Initialize with Prototype
HRESULT CTexture::InitComponent_Proto(TEXTURE_TYPE _eType, const _tchar* _szFilePath, _uint _uiTextureCount){
	_tchar szFilePath[MAX_PATH] = L"";

	// �̹��� ���� ��ŭ ����ũ�� ����
	m_uiTextureCount = _uiTextureCount;
	m_TextureVec.reserve(_uiTextureCount);

	// �ؽ��� ����
	for(size_t i = 0; i < _uiTextureCount; ++i){
		LPDIRECT3DBASETEXTURE9 pTexture = nullptr;
		HRESULT hr = 0;

		wsprintf(szFilePath, _szFilePath, i);

		switch(_eType){
		case TEXTURE_GENERAL:	// �Ϲ����� �ؽ���
			hr = D3DXCreateTextureFromFile(CComponent::GetGraphicDevice(), szFilePath, reinterpret_cast<LPDIRECT3DTEXTURE9*>(&pTexture));
			break;
			
		case TEXTURE_CUBE:		// ť�� �ؽ���
			hr = D3DXCreateCubeTextureFromFile(CComponent::GetGraphicDevice(), szFilePath, reinterpret_cast<LPDIRECT3DCUBETEXTURE9*>(&pTexture));
			break;
		default:
			break;
		}

		if(FAILED(hr))
			return E_FAIL;

		// �����̳ʿ� ����
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

// ���� ���� �Լ�
CComponent * CTexture::CreateClone(void* _pArgument){
	CComponent* pInstance = new CTexture(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		MSG_BOX(L"CTexture Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// ������Ÿ�� ���� �Լ�
CTexture * CTexture::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, TEXTURE_TYPE _eType, const _tchar* _szFilePath, _uint _uiTextureCount/* = 1*/){
	CTexture* pInstance = new CTexture(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_eType, _szFilePath, _uiTextureCount))){
		MSG_BOX(L"CTexture Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}
