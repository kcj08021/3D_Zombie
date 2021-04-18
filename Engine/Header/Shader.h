#ifndef __SHADER_H__
#define __SHADER_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final:
	public CComponent{
private:
	// 생성자
	explicit CShader(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CShader(const CShader& _Shader);
	// 소멸자
	virtual ~CShader() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(_tchar* _szShadeFilePath);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;


	// Set
public:
	// 셰이더에 전역변수 세팅
	HRESULT SetValue(D3DXHANDLE _szVarName, void* _pValue, _uint _uiSizeValue);
	// 셰이더에 텍스쳐 세팅
	HRESULT SetTexture(D3DXHANDLE _szVarName, LPDIRECT3DBASETEXTURE9 _pTexture);
	// 셰이더에 bool형 변수 전달
	HRESULT SetBool(D3DXHANDLE _szVarName, _bool _bool);
public:
	HRESULT CommitChange();
	// 셰이더로 그리기 시작
	HRESULT BeginShader();
	// 셰이더로 그리기 끝
	HRESULT EndShader();
	// 셰이더 패스 시작
	HRESULT BeginPass(_uint _uiPassNum);
	// 셰이더 패스 끝
	HRESULT EndPass();

protected:

private:
	// 셰이더 파일을 컴파일한 셰이더를 이용하기 위한 인터페이스를 가진 객체의 포인터 변수
	LPD3DXEFFECT m_pShader = nullptr;
	// 셰이더 파일 컴파일 중 생성되는 메세지를 받기 위한 버퍼 포인터
	LPD3DXBUFFER m_pErrorMassage = nullptr;
	// 패스 갯수를 받기 위한 변수
	_uint m_uiPassCount = 0;

public:
	// 생성 함수
	static CShader* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _tchar* _szShadeFilePath);
	// 복제 함수
	virtual CComponent * CreateClone(void* _pArgument) override;
public:
	// Free
	virtual void Free();
};

END

#endif // !__SHADER_H__
