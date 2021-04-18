#include "stdafx.h"
#include "../Header/Client_Headers.h"
#include "../Header/Weapon.h"
#include "../Header/Grenade.h"
#include "../Header/MuzzleEffect.h"

USING(Client)

CWeapon::CWeapon(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_pShaderCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pPlayerPicker(nullptr)
	, m_pFirstPersonModelCom(nullptr)
	, m_pFirstPersonKnifeCom(nullptr)
	, m_pThirdPersonModelCom(nullptr)
	, m_bIsThirdPerson(false)
	, m_szWaeponName(nullptr)
	, m_bIsMeleeAttack(false)
	, m_iMaxOneMagazine(0)
	, m_iCurrentMagazine(0)
	, m_iMaxAmmo(0)
	, m_iCurrentAmmo(0)
	, m_fFiringRate(0.f)
	, m_pFiringTimer(nullptr)
	, m_fDamage(0.f)
	, m_dAccFiringTime(0.0)
	, m_pMuzzleLight(nullptr){
}

CWeapon::CWeapon(const CWeapon & _Weapon)
	: CComponent(_Weapon)
	, m_pShaderCom(_Weapon.m_pShaderCom)
	, m_pTransformCom(nullptr)
	, m_pPlayerPicker(nullptr)
	, m_pFirstPersonModelCom(_Weapon.m_pFirstPersonModelCom)
	, m_pFirstPersonKnifeCom(_Weapon.m_pFirstPersonKnifeCom)
	, m_pThirdPersonModelCom(_Weapon.m_pThirdPersonModelCom)
	, m_szWaeponName(_Weapon.m_szWaeponName)
	, m_bIsThirdPerson(false)
	, m_bIsMeleeAttack(false)
	, m_eWeaponType(_Weapon.m_eWeaponType)
	, m_iMaxOneMagazine(_Weapon.m_iMaxOneMagazine)
	, m_iCurrentMagazine(_Weapon.m_iCurrentMagazine)
	, m_iMaxAmmo(_Weapon.m_iMaxAmmo)
	, m_iCurrentAmmo(_Weapon.m_iCurrentAmmo)
	, m_fFiringRate(_Weapon.m_fFiringRate)
	, m_pFiringTimer(nullptr)
	, m_fDamage(_Weapon.m_fDamage)
	, m_dAccFiringTime(0.0)
	, m_pMuzzleLight(nullptr){
	SafeAddRef(m_pTransformCom);
	SafeAddRef(m_pShaderCom);
	SafeAddRef(m_pThirdPersonModelCom);
	SafeAddRef(m_pFirstPersonModelCom);
	SafeAddRef(m_pFirstPersonKnifeCom);
}

// Initialize with Prototype
HRESULT CWeapon::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 셰이더 컴포넌트 추가
	m_pShaderCom = dynamic_cast<CShader*>(pManagement->AddCloneComponent(L"DefaultShaderCom_Proto", STATIC_SCENE));
	if(nullptr == m_pShaderCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	SafeRelease(pManagement);

	return NOERROR;
}
// Initialize with Clone
HRESULT CWeapon::InitComponent_Clone(void* _pArgument){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 트랜스폼 컴포넌트 추가
	m_pTransformCom = dynamic_cast<CTransform*>(pManagement->AddCloneComponent(L"TransformCom_Proto", STATIC_SCENE));
	if(nullptr == m_pTransformCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 1인칭 근접공격용 칼 추가
	m_pFirstPersonKnifeCom = dynamic_cast<CStaticMesh*>(pManagement->AddCloneComponent(L"viewmodel_knife_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonKnifeCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	D3DXFRAME_DERIVED* pFrame = m_pFirstPersonModelCom->GetFrame("tag_knife_attach");
	if(nullptr != pFrame)
		m_pKnifeFrameUpdateMatrix = &pFrame->CombinedTransformationMatrix;

	m_pVeiwmodelGrenade = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_m67_grenade_Proto", STAGE_SCENE));
	if(nullptr == m_pVeiwmodelGrenade){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 머즐 위치 라이트 추가
	pFrame = m_pFirstPersonModelCom->GetFrame("tag_flash");
	if(nullptr != pFrame){
		m_pMuzzleFlashMatrix = &pFrame->CombinedTransformationMatrix;
	}
	D3DLIGHT9 tLight;
	tLight.Type = D3DLIGHT_POINT;
	tLight.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	tLight.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLight.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLight.Range = 10.f;
	tLight.Position = _vec3();

	pManagement->AddLight(CComponent::GetGraphicDevice(), &tLight, &m_pMuzzleLight);
	if(nullptr == m_pMuzzleLight)
		return E_FAIL;

	m_pMuzzleLight->SetTurnLight(false);

	m_pFiringTimer = CTimer::Create();

	SafeRelease(pManagement);

	return NOERROR;
}
// LateInitialize
HRESULT CWeapon::LateInitWeapon(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	auto pList = pManagement->GetObjectList(STAGE_SCENE, L"MuzzleEffect");

	m_pMuzzleEffect = reinterpret_cast<CMuzzleEffect*>(pList->front());
	if(nullptr != m_pMuzzleEffect){
		m_pMuzzleEffect->SetEffectStatus(false);
	}
	SafeRelease(pManagement);
	m_bIsLateInit = true;

	return this->LateInitWeapon();
}
// Updatesds
HRESULT CWeapon::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
	m_dAnimationTime += _dDeltaTime;

	if(nullptr != m_pMuzzleLight)
		m_pMuzzleLight->SetTurnLight(false);
	if(nullptr != m_pMuzzleEffect)
		m_pMuzzleEffect->SetEffectStatus(false);
	m_bIsFire = false;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	_matrix matCameraWorld, matWorld;
	pManagement->GetCameraWorldMatrix(&matCameraWorld);


	matWorld = *m_pMuzzleFlashMatrix * matCameraWorld;

	_vec3 vLightPos(matWorld.m[3]);

	_matrix matView;
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	D3DXVec3TransformCoord(&vLightPos, &vLightPos, &matView);
	vLightPos *= c_fScaleConst;
	D3DXVec3TransformCoord(&vLightPos, &vLightPos, &matCameraWorld);

	if(nullptr != m_pMuzzleLight)
		m_pMuzzleLight->SetLightPos(vLightPos);
	if(nullptr != m_pMuzzleEffect)
		m_pMuzzleEffect->SetEffectPosition(vLightPos);

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CWeapon::RenderWeapon(){
	if(FAILED(SettingShader()))
		return E_FAIL;

	// 셰이더를 통하여 렌더 시작
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(1);

	// 1인칭 렌더의 경우 깊이버퍼를 초기화
	if(FAILED(CComponent::GetGraphicDevice()->Clear(0, nullptr, D3DCLEAR_ZBUFFER, 0, 1.f, 0)))
		return E_FAIL;

	FirstPersonModelRender();

	if(m_bIsMeleeAttack)
		FirstPersonKnifeRender();


	// 셰이더를 통한 렌더 종료
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	return NOERROR;
}

HRESULT CWeapon::Fire(const POINT& _pt, CPicker* _pPicker){
	if(0 >= m_iCurrentMagazine)
		return E_FAIL;

	--m_iCurrentMagazine;
	m_bIsFire = true;
	if(nullptr != m_pMuzzleLight)
		m_pMuzzleLight->SetTurnLight(true);
	if(nullptr != m_pMuzzleEffect)
		m_pMuzzleEffect->SetEffectStatus(true);

	_vec3 vRayDir, vRayPos;
	_pPicker->TransformMousePos(_pt, vRayDir, vRayPos);

	if(_pPicker->CheckPicking(vRayDir, vRayPos)){
		auto listPicking = _pPicker->GetPickingResult();
		auto list_iter = listPicking.begin();
		for(int i = 0; i < 2; ++i){
			if(list_iter == listPicking.end())
				break;
			CNonPlayer* pNPC = dynamic_cast<CNonPlayer*>((list_iter++)->pGameObject);
			if(nullptr != pNPC)
				pNPC->CheckHit(vRayDir, vRayPos, m_fDamage);
		}
	}
	
	return NOERROR;
}

HRESULT CWeapon::Reload(){
	if(m_iCurrentMagazine == m_iMaxOneMagazine
		|| 0 == m_iCurrentAmmo)
		return E_FAIL;

	// 현재 남은 탄창의 탄약을 남은 전체 탄약 더함
	m_iCurrentAmmo += m_iCurrentMagazine;

	// 현재 탄창 비움
	m_iCurrentMagazine = 0;

	// 전체 탄약에서 한 탄창만큼 뺌
	m_iCurrentAmmo -= m_iMaxOneMagazine;

	// 현재 탄창을 채움
	m_iCurrentMagazine = m_iMaxOneMagazine;

	// 마지막에 계산된 전체에 남은 탄약이 -라면 현재 탄창에 더해줌(부족한 탄약만큼 빼줌)
	if(0 > m_iCurrentAmmo){
		m_iCurrentMagazine += m_iCurrentAmmo;
		m_iCurrentAmmo = 0;
	}

	return NOERROR;
}

_bool CWeapon::Grenade(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return false;

	SafeAddRef(pManagement);

	if(!m_bIsGrenade && pManagement->IsKeyDown(DIK_G)){
		m_pVeiwmodelGrenade->SetUpAnimationSet(GRENADE_PULLPIN, 1.5f);
		SafeRelease(pManagement);
		return true;
	}

	SafeRelease(pManagement);

	return false;
}

HRESULT CWeapon::UpdateGrenadeState(_double _dDeltaTime){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return false;

	SafeAddRef(pManagement);

	switch(m_pVeiwmodelGrenade->GetCurrentAnaimationSet()){
	case GRENADE_PULLPIN:
		if(m_pVeiwmodelGrenade->IsFinished())
			m_pVeiwmodelGrenade->SetUpAnimationSet(GRENADE_THROWING_IDLE, 1.5f);
		break;
	case GRENADE_THROWING_IDLE:
		if(pManagement->IsKeyPressing(DIK_G)){
			SafeRelease(pManagement);
			return NOERROR;
		} else{
			CGrenade::GRENADE_DESC tDesc;
			pManagement->GetCameraWorldMatrix(&tDesc.matWorld);
			tDesc.eType = CGrenade::HAND;
			pManagement->AddCloneObjectToLayer(L"Grenade_Proto", STAGE_SCENE, L"Grenade", &tDesc);
			m_pVeiwmodelGrenade->SetUpAnimationSet(GRENADE_THROW, 1.5f);
		}
		break;
	case GRENADE_THROW:
		if(m_pVeiwmodelGrenade->IsFinished()){
			m_bIsGrenade = false;
		}
		break;
	}

	m_pVeiwmodelGrenade->PlayAnimationSet(_dDeltaTime);

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CWeapon::FirstPersonModelRender(){
	CDynamicMesh* pDynamicMesh = nullptr;

	if(m_bIsGrenade)
		pDynamicMesh = m_pVeiwmodelGrenade;
	else
		pDynamicMesh = m_pFirstPersonModelCom;

	_uint uiMeshContainerCount = pDynamicMesh->GetMeshContainerCount();

	for(size_t i = 0; i < uiMeshContainerCount; ++i){
		_ulong dwSubsetCount = pDynamicMesh->GetSubsetCount(i);

		pDynamicMesh->UpdateSkinnedMesh(i);

		for(_ulong j = 0; j < dwSubsetCount; ++j){
			LPDIRECT3DTEXTURE9 Texture = pDynamicMesh->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_DIFFUSE);
			m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

			Texture = pDynamicMesh->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_NORMAL);
			// 노말맵이 없으면 노말매핑 끔
			if(nullptr != Texture){
				m_pShaderCom->SetBool("g_bIsNormalMapping", true);
				m_pShaderCom->SetTexture("g_NormalTexture", Texture);
			} else{
				m_pShaderCom->SetBool("g_bIsNormalMapping", false);
			}

			Texture = pDynamicMesh->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_SPECULAR);
			// 스펙큘러맵이 없으면 스펙큘러매핑 끔
			if(nullptr != Texture){
				m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
				m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
			} else{
				m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
			}

			m_pShaderCom->CommitChange();

			pDynamicMesh->RenderMesh(i, j);
		}
	}

	return NOERROR;
}

HRESULT CWeapon::FirstPersonKnifeRender(){
	if(FAILED(SettingKnifeShader()))
		return E_FAIL;

	_ulong dwSubsetCount = m_pFirstPersonKnifeCom->GetSubsetCount();

	for(_ulong i = 0; i < dwSubsetCount; ++i){
		LPDIRECT3DTEXTURE9 Texture = m_pFirstPersonKnifeCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_DIFFUSE);
		m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

		Texture = m_pFirstPersonModelCom->GetMaterialTexture(i, i, MESH_TEXTURE::TYPE_NORMAL);
		// 노말맵이 없으면 노말매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsNormalMapping", true);
			m_pShaderCom->SetTexture("g_NormalTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsNormalMapping", false);
		}

		Texture = m_pFirstPersonModelCom->GetMaterialTexture(i, i, MESH_TEXTURE::TYPE_SPECULAR);
		// 스펙큘러맵이 없으면 스펙큘러매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
			m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
		}

		m_pShaderCom->CommitChange();

		m_pFirstPersonKnifeCom->RenderMesh(i);
	}

	return NOERROR;
}

// 셰이더의 전역변수 설정
HRESULT CWeapon::SettingShader(){
	if(nullptr == m_pShaderCom)
		return E_FAIL;

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 카메라의 월드행렬 가져옴
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);

	// 각 변환행렬 변수들
	_matrix matWorld, matView, matProj;

	// 월드 행렬은 1인칭인 경우 카메라의 월드행렬을 사용
	matWorld = matCameraWorld;

	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// 셰이더에 객체의 월드, 뷰, 투영 행렬 전달
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CWeapon::SettingKnifeShader(){
	if(nullptr == m_pShaderCom)
		return E_FAIL;

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 카메라의 월드행렬 가져옴
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);

	_matrix matWorld = *m_pKnifeFrameUpdateMatrix * matCameraWorld;

	// 셰이더에 나이프의 월드 전달
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));

	SafeRelease(pManagement);

	return NOERROR;
}

void CWeapon::Free(){
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pThirdPersonModelCom);
	SafeRelease(m_pFirstPersonModelCom);
	SafeRelease(m_pFirstPersonKnifeCom);
	SafeRelease(m_pVeiwmodelGrenade);
	SafeRelease(m_pFiringTimer);

	CComponent::Free();
}
