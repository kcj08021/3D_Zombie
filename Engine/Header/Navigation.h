#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "Component.h"

BEGIN(Engine)
class CCell;
class CShader;
class ENGINE_DLL CNavigation:
	public CComponent{
private:
	// ������
	explicit CNavigation(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CNavigation(const CNavigation& _Navigation);
	// �Ҹ���
	virtual ~CNavigation() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(const _tchar* _szFilePath);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);

public:
	_bool MovingCheck(_vec3& _vMoveResult);
	// �׺�޽��� ����
	HRESULT RenderNaviMash();

private:
	// �׺���̼� �޽� �� �ﰢ������ �����ϴ� �����̳�
	vector<CCell*> m_vecCell;
	typedef vector<CCell*> CELL_VECTOR;

	CShader* m_pShaderCom = nullptr;
	// �ﰢ���� �� ����
	_uint m_uiCellCount = 0;
	// ���� �θ� ��ü�� �ִ� �׺� �޽��� �ε���
	_uint m_uiCurrentIndex = 0;

private:
	// �� �ﰢ������ �̿��� �����ϴ� �Լ�
	HRESULT SetttingNeigbor();

public:
	// ���� ���� �Լ�(static)
	static CNavigation* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szFilePath);
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* _pArgument);
	
public:
	// Free
	virtual void Free();
};

END

#endif // !__NAVIGATION_H__
