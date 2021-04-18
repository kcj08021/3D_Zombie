#ifndef __ROTATION_CONTAINER_H__
#define __ROTATION_CONTAINER_H__

#include "Component.h"

BEGIN(Tool)

class CRotationContainer final :
	public CComponent{
private:
	explicit CRotationContainer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CRotationContainer(const CRotationContainer& _RotationContainer);
	virtual ~CRotationContainer() = default;

private:
	virtual HRESULT InitComponent_Proto();
	virtual HRESULT InitComponent_Clone(void * _pArgument) override;

public:
	void SetRotationValue(const _float*& _fRotationValue);
	void SetRotationAxis(_uint _uiAxis, _float fRotate);
	
public:
	void GetRotationValue(_float*& _fRotationValue);
	_float GetRotationAxis(_uint _uiAxis);

private:
	_float m_RotationValue[3];

public:
	static CRotationContainer* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	virtual CComponent * CreateClone(void * _pArgument) override;

public:
	virtual void Free();
};

END

#endif // !__ROTATION_CONTAINER_H__
