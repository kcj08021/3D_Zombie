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
	// ���̴� ���� ������ ���н�
	// D3DXCreateEffectFromFile()�� ������������ LPD3DXBUFFER�� ���̴� ������ �������Ҷ� ��µǴ� �޼����� ������
	if(nullptr == m_pShader &&
		nullptr != m_pErrorMassage){
		// �޾ƿ� ������ ������
		_int iSize = m_pErrorMassage->GetBufferSize();
		// �޾ƿ� ������ ù ���۰�(������)
		void* pAck = m_pErrorMassage->GetBufferPointer();

		if(nullptr != pAck){
			char* szDebugMsg = new char[iSize];
			sprintf_s(szDebugMsg, iSize, reinterpret_cast<char*>(pAck));
			// ����� ���â�� ���ۿ� ��ϵ� �޼��� ���
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

// ���̴��� ��������(��� ��) ����
HRESULT CShader::SetValue(D3DXHANDLE _szVarName, void* _pValue, _uint _uiSizeValue ){
	return m_pShader->SetValue(_szVarName, _pValue, _uiSizeValue);
}

// ���̴��� �ؽ��� ����
HRESULT CShader::SetTexture(D3DXHANDLE _szVarName, LPDIRECT3DBASETEXTURE9 _pTexture){
	return m_pShader->SetTexture(_szVarName, _pTexture);
}

// ���̴��� bool�� ���� ����
HRESULT CShader::SetBool(D3DXHANDLE _szVarName, _bool _bool){
	return m_pShader->SetBool(_szVarName, _bool);
}

HRESULT CShader::CommitChange(){
	if(nullptr == m_pShader)
		return E_FAIL;

	return m_pShader->CommitChanges();
}

// ���̴��� �׸��� ����
HRESULT CShader::BeginShader(){
	m_pShader->Begin(&m_uiPassCount, 0);

	return NOERROR;
}

// ���̴��� �׸��� ��
HRESULT CShader::EndShader(){
	m_pShader->End();
	return NOERROR;
}

// ���̴� �н� ����
HRESULT CShader::BeginPass(_uint _uiPassNum){
	if(_uiPassNum >= m_uiPassCount)
		return E_FAIL;

	m_pShader->BeginPass(_uiPassNum);

	return NOERROR;
}

// ���̴� �н� ��
HRESULT CShader::EndPass(){
	m_pShader->EndPass();

	return NOERROR;
}

// ���� �Լ�
CShader * CShader::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _tchar * _szShadeFilePath){
	CShader* pInstnace = new CShader(_pGraphicDevice);
	if(FAILED(pInstnace->InitComponent_Proto(_szShadeFilePath))){
		MSG_BOX(L"CShader Create Failure");
		SafeRelease(pInstnace);
	}
	
	return pInstnace;
}

// ���� �Լ�
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
