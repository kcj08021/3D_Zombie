#include "..\Header\Shader.h"

CShader::CShader(LPDIRECT3DDEVICE9 _pGraphicDevice) 
	: CComponent(_pGraphicDevice)
	, m_pShader(nullptr)
	, m_pErrorMassage(nullptr)
	, m_uiPassCount(0){
}

CShader::CShader(const CShader & _Shader)
	: CComponent(_Shader) 
	, m_pShader(_Shader.m_pShader)
	, m_pErrorMassage(_Shader.m_pErrorMassage){
	SafeAddRef(_Shader.m_pErrorMassage);
	SafeAddRef(_Shader.m_pShader);
}

// Initialize with Prototype
HRESULT CShader::InitComponent_Proto(_tchar * _szShadeFilePath){
	LPDIRECT3DDEVICE9 pGraphicDevice = CComponent::GetGraphicDevice();
	if(nullptr == pGraphicDevice)
		return E_FAIL;

	SafeAddRef(pGraphicDevice);

	_ulong dwShaderFlag = 0;
	HRESULT hr = 0;

#ifdef _DEBUG
	dwShaderFlag |= D3DXSHADER_DEBUG;
#endif // _DEBUG

	hr = D3DXCreateEffectFromFile(pGraphicDevice, _szShadeFilePath, nullptr, nullptr, dwShaderFlag, nullptr, &m_pShader, &m_pErrorMassage);
	// 셰이더 파일 컴파일 실패시
	// D3DXCreateEffectFromFile()의 마지막인자인 LPD3DXBUFFER는 셰이더 파일을 컴파일할때 출력되는 메세지를 보관함
	if(nullptr == m_pShader &&
		nullptr != m_pErrorMassage){
		// 받아온 버퍼의 사이즈
		_int iSize = m_pErrorMassage->GetBufferSize();
		// 받아온 버퍼의 첫 시작값(포인터)
		void* pAck = m_pErrorMassage->GetBufferPointer();

		if(nullptr != pAck){
			char* szDebugMsg = new char[iSize];
			sprintf_s(szDebugMsg, iSize, reinterpret_cast<char*>(pAck));
			// 디버그 출력창에 버퍼에 기록된 메세지 출력
			OutputDebugStringA(szDebugMsg);

			SafeDelete_Array(szDebugMsg);
		}
	}

	SafeRelease(pGraphicDevice);

	return hr;
}

// Initialize with Clone
HRESULT CShader::InitComponent_Clone(void* _pArgument){
	return NOERROR;
}

// 셰이더에 전역변수(행렬 등) 세팅
HRESULT CShader::SetValue(D3DXHANDLE _szVarName, void* _pValue, _uint _uiSizeValue ){
	return m_pShader->SetValue(_szVarName, _pValue, _uiSizeValue);
}

// 셰이더에 텍스쳐 세팅
HRESULT CShader::SetTexture(D3DXHANDLE _szVarName, LPDIRECT3DBASETEXTURE9 _pTexture){
	return m_pShader->SetTexture(_szVarName, _pTexture);
}

// 셰이더에 bool형 변수 전달
HRESULT CShader::SetBool(D3DXHANDLE _szVarName, _bool _bool){
	return m_pShader->SetBool(_szVarName, _bool);
}

HRESULT CShader::CommitChange(){
	if(nullptr == m_pShader)
		return E_FAIL;

	return m_pShader->CommitChanges();
}

// 셰이더로 그리기 시작
HRESULT CShader::BeginShader(){
	m_pShader->Begin(&m_uiPassCount, 0);

	return NOERROR;
}

// 셰이더로 그리기 끝
HRESULT CShader::EndShader(){
	m_pShader->End();
	return NOERROR;
}

// 셰이더 패스 시작
HRESULT CShader::BeginPass(_uint _uiPassNum){
	if(_uiPassNum >= m_uiPassCount)
		return E_FAIL;

	m_pShader->BeginPass(_uiPassNum);

	return NOERROR;
}

// 셰이더 패스 끝
HRESULT CShader::EndPass(){
	m_pShader->EndPass();

	return NOERROR;
}

// 생성 함수
CShader * CShader::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _tchar * _szShadeFilePath){
	CShader* pInstnace = new CShader(_pGraphicDevice);
	if(FAILED(pInstnace->InitComponent_Proto(_szShadeFilePath))){
		MSG_BOX(L"CShader Create Failure");
		SafeRelease(pInstnace);
	}
	
	return pInstnace;
}

// 복제 함수
CComponent * CShader::CreateClone(void* _pArgument){
	CComponent* pInstnace = new CShader(*this);
	if(FAILED(pInstnace->InitComponent_Clone(_pArgument))){
		MSG_BOX(L"CShader Clone Failure");
		SafeRelease(pInstnace);
	}

	return pInstnace;
}

// Free
void CShader::Free(){
	SafeRelease(m_pErrorMassage);
	SafeRelease(m_pShader);

	CComponent::Free();
}
