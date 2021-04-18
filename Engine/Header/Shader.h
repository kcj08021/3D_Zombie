#ifndef __SHADER_H__
#define __SHADER_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final:
	public CComponent{
private:
	// ������
	explicit CShader(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CShader(const CShader& _Shader);
	// �Ҹ���
	virtual ~CShader() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(_tchar* _szShadeFilePath);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;


	// Set
public:
	// ���̴��� �������� ����
	HRESULT SetValue(D3DXHANDLE _szVarName, void* _pValue, _uint _uiSizeValue);
	// ���̴��� �ؽ��� ����
	HRESULT SetTexture(D3DXHANDLE _szVarName, LPDIRECT3DBASETEXTURE9 _pTexture);
	// ���̴��� bool�� ���� ����
	HRESULT SetBool(D3DXHANDLE _szVarName, _bool _bool);
public:
	HRESULT CommitChange();
	// ���̴��� �׸��� ����
	HRESULT BeginShader();
	// ���̴��� �׸��� ��
	HRESULT EndShader();
	// ���̴� �н� ����
	HRESULT BeginPass(_uint _uiPassNum);
	// ���̴� �н� ��
	HRESULT EndPass();

protected:

private:
	// ���̴� ������ �������� ���̴��� �̿��ϱ� ���� �������̽��� ���� ��ü�� ������ ����
	LPD3DXEFFECT m_pShader = nullptr;
	// ���̴� ���� ������ �� �����Ǵ� �޼����� �ޱ� ���� ���� ������
	LPD3DXBUFFER m_pErrorMassage = nullptr;
	// �н� ������ �ޱ� ���� ����
	_uint m_uiPassCount = 0;

public:
	// ���� �Լ�
	static CShader* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _tchar* _szShadeFilePath);
	// ���� �Լ�
	virtual CComponent * CreateClone(void* _pArgument) override;
public:
	// Free
	virtual void Free();
};

END

#endif // !__SHADER_H__
