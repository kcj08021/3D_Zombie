#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final: public CComponent{
public:
	enum TEXTURE_TYPE{ TEXTURE_GENERAL, TEXTURE_CUBE, TEXTURE_END};
private:
	// 생성자
	explicit CTexture(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CTexture(const CTexture& _Texture);
	// 소멸자
	virtual ~CTexture() = default;

	// Get
public:
	// 텍스쳐를 반환하는 Get 함수
	const LPDIRECT3DBASETEXTURE9 GetTexture(_uint _uiIndex = 0) const{
		if(_uiIndex >= m_uiTextureCount)
			return nullptr;
		return m_TextureVec[_uiIndex];
	}
	// 텍스쳐의 수를 반환하는 함수
	_uint GetTextureCount(){
		return m_uiTextureCount;
	}

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(TEXTURE_TYPE _eType, const _tchar* _szFilePath, _uint _uiTextureCount);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);

protected:

private:
	// 텍스쳐의 갯수(vector의 사이즈와 같음)
	_uint m_uiTextureCount = 0;
	// 텍스쳐를 보관하기 위한 컨테이너 변수(vector)
	vector<LPDIRECT3DBASETEXTURE9> m_TextureVec;
	typedef vector<LPDIRECT3DBASETEXTURE9> TEXTURE_VEC;

public:
	// Free
	virtual void Free();
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);

	//static 함수
public:
	// 프로토타입 생성 함수
	static CTexture* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, TEXTURE_TYPE _eType, const _tchar* _szFilePath, _uint _uiTextureCount = 1);
};

END

#endif // !__TEXTURE_H__
