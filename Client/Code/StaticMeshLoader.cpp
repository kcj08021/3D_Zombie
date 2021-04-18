#include "stdafx.h"
#include "..\Header\StaticMeshLoader.h"
#include "..\Header\StaticMeshObject.h"
#include "Management.h"

USING(Client)

CStaticMeshLoader::CStaticMeshLoader(LPDIRECT3DDEVICE9 _pGraphicDevice)
	:CComponent(_pGraphicDevice)
	, m_uiSceneNumber(0){
}

CStaticMeshLoader::CStaticMeshLoader(const CStaticMeshLoader & _StaticMeshLoader)
	: CComponent(_StaticMeshLoader)
	, m_listObjectSetting(_StaticMeshLoader.m_listObjectSetting)
	, m_uiSceneNumber(_StaticMeshLoader.m_uiSceneNumber){
}

HRESULT CStaticMeshLoader::InitComponent_Proto(const _tchar * _szFilePath, _uint _uiSceneNumber){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement){
		return E_FAIL;
	}
	SafeAddRef(pManagement);

	m_uiSceneNumber = _uiSceneNumber;

	HANDLE hFile = CreateFile(_szFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(0 == hFile){
		return E_FAIL;
	}

	_ulong dwByte;
	_bool bReadFinish = false;
	_tchar szInfo[12][128];
	_uint uiIndex = 0;
	while(true){
		_uint uiStringIndex = 0;
		_tchar str[128];
		::ZeroMemory(str, sizeof(_tchar) * 128);
		while(true){
			_tchar temp;
			ReadFile(hFile, &temp, sizeof(_tchar), &dwByte, 0);
			if(dwByte == 0){
				bReadFinish = true;
				break;
			}

			if(temp == L'|' || temp == L'\n'){
				uiStringIndex = 0;
				lstrcpy(szInfo[uiIndex++], str);
				::ZeroMemory(str, sizeof(_tchar) * 128);
			} else{
				str[uiStringIndex++] = temp;
			}

			if(temp == L'\n')
				break;
		}
		uiIndex = 0;

		if(!bReadFinish){

			OBJECT_SETTING_INFO Info;

			Info.fScaleX = static_cast<float>(_ttof(szInfo[2]));
			Info.fScaleY = static_cast<float>(_ttof(szInfo[3]));
			Info.fScaleZ = static_cast<float>(_ttof(szInfo[4]));
			Info.fRotateX = static_cast<float>(_ttof(szInfo[5]));
			Info.fRotateY = static_cast<float>(_ttof(szInfo[6]));
			Info.fRotateZ = static_cast<float>(_ttof(szInfo[7]));
			Info.fPosX = static_cast<float>(_ttof(szInfo[8]));
			Info.fPosY = static_cast<float>(_ttof(szInfo[9]));
			Info.fPosZ = static_cast<float>(_ttof(szInfo[10]));
			lstrcpy(Info.szProto, szInfo[11]);
			lstrcpy(Info.szLayer, szInfo[0]);

			m_listObjectSetting.push_back(Info);
		} else
			break;
	}
	CloseHandle(hFile);

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CStaticMeshLoader::InitComponent_Clone(void * _pArgument){
	return NOERROR;
}

HRESULT CStaticMeshLoader::CreateStaticMeshObject(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);
	for(auto& elem : m_listObjectSetting){
		_matrix matScale, matRotateX, matRotateY, matRotateZ, matTrans, matWorld;
		D3DXMatrixScaling(&matScale, elem.fScaleX, elem.fScaleY, elem.fScaleZ);
		D3DXMatrixRotationX(&matRotateX, D3DXToRadian(elem.fRotateX));
		D3DXMatrixRotationY(&matRotateY, D3DXToRadian(elem.fRotateY));
		D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(elem.fRotateZ));
		D3DXMatrixTranslation(&matTrans, elem.fPosX, elem.fPosY, elem.fPosZ);

		matWorld = matScale * matRotateX * matRotateY * matRotateZ * matTrans;

		CStaticMeshObject::STATIC_MESH_DESC tDesc;
		tDesc.szStaticMeshComName = elem.szProto;
		tDesc.tTransformDesc.fRotationPerSec = 0.f;
		tDesc.tTransformDesc.fSpeedPerSec = 0.f;
		tDesc.tTransformDesc.pMatrixWorldSetting = &matWorld;

		if(FAILED(pManagement->AddCloneObjectToLayer(L"StaticMeshObject_Proto", m_uiSceneNumber, elem.szLayer, &tDesc)))
			return E_FAIL;
	}

	SafeRelease(pManagement);
	return NOERROR;
}

CStaticMeshLoader * CStaticMeshLoader::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar * _szFilePath, _uint _uiSceneNumber){
	CStaticMeshLoader* pInstance = new CStaticMeshLoader(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_szFilePath, _uiSceneNumber))){
		SafeRelease(pInstance);
		MSG_BOX(L"CStaticMeshLoader Create Failure");
	}

	return pInstance;
}

CComponent * CStaticMeshLoader::CreateClone(void * _pArgument){
	CComponent* pInstance = new CStaticMeshLoader(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CStaticMeshLoader Clone Failure");
	}

	return pInstance;
}

void CStaticMeshLoader::Free(){
	CComponent::Free();
}
