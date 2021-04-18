#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final: public CComponent{
public:
	enum TEXTURE_TYPE{ TEXTURE_GENERAL, TEXTURE_CUBE, TEXTURE_END};
private:
	// ������
	explicit CTexture(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CTexture(const CTexture& _Texture);
	// �Ҹ���
	virtual ~CTexture() = default;

	// Get
public:
	// �ؽ��ĸ� ��ȯ�ϴ� Get �Լ�
	const LPDIRECT3DBASETEXTURE9 GetTexture(_uint _uiIndex = 0) const{
		if(_uiIndex >= m_uiTextureCount)
			return nullptr;
		return m_TextureVec[_uiIndex];
	}
	// �ؽ����� ���� ��ȯ�ϴ� �Լ�
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
	// �ؽ����� ����(vector�� ������� ����)
	_uint m_uiTextureCount = 0;
	// �ؽ��ĸ� �����ϱ� ���� �����̳� ����(vector)
	vector<LPDIRECT3DBASETEXTURE9> m_TextureVec;
	typedef vector<LPDIRECT3DBASETEXTURE9> TEXTURE_VEC;

public:
	// Free
	virtual void Free();
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* _pArgument);

	//static �Լ�
public:
	// ������Ÿ�� ���� �Լ�
	static CTexture* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, TEXTURE_TYPE _eType, const _tchar* _szFilePath, _uint _uiTextureCount = 1);
};

END

#endif // !__TEXTURE_H__
