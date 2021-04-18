#include "..\Header\VIBufferTerrain.h"

// 생성자
CVIBufferTerrain::CVIBufferTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CVIBuffer(_pGraphicDevice)
	, m_uiVerticesCountX(0)
	, m_uiVerticesCountZ(0)
	, m_fInterval(0.f){
}

// 복사 생성자
CVIBufferTerrain::CVIBufferTerrain(const CVIBufferTerrain & _VIBufferTerrain)
	: CVIBuffer(_VIBufferTerrain)
	, m_uiVerticesCountX(_VIBufferTerrain.m_uiVerticesCountX)
	, m_uiVerticesCountZ(_VIBufferTerrain.m_uiVerticesCountZ)
	, m_fInterval(_VIBufferTerrain.m_fInterval){
}

// Initialize with Prototype(지형 가로세로 크기)
HRESULT CVIBufferTerrain::InitComponent_Proto(_uint _uiVerticesCountX, _uint _uiVerticesCountZ, _float _fInterval/* = 1.f*/){
	if(1 >= _uiVerticesCountX
		|| 1 >= _uiVerticesCountZ
		|| 0.1 >= _fInterval)
		return E_FAIL;

	// 지형 크기 정보 보관
	m_uiVerticesCountX = _uiVerticesCountX;
	m_uiVerticesCountZ = _uiVerticesCountZ;
	m_fInterval = _fInterval;
	
	// 버텍스 구조체의 사이즈 설정
	m_uiStride = sizeof(_VERTEX_TEX_NORMAL);
	// 버텍스의 갯수 설정
	m_uiVerticesCount = _uiVerticesCountX * _uiVerticesCountZ;
	// FVF 설정
	m_ulFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	// 버텍스로 그려질 삼각형의 갯수 설정
	m_uiPrimitiveCount = (_uiVerticesCountX - 1) * (_uiVerticesCountZ - 1) * 2;

	// 폴리곤의 갯수만큼 버텍스 위치 정보를 보관하는 배열 할당
	m_pPolygonPositionArray = new POLYGON_POSITION[m_uiPrimitiveCount];

	// 인덱스 버퍼 원소 3개의 사이즈
	m_uiPolygonIndicesSize = sizeof(INDEX32);
	// 인덱스 버퍼의 포맷 설정
	m_eFormat = D3DFMT_INDEX32;

	// 설정한 정보로 버텍스 버퍼와 인덱스 버퍼 할당
	if(FAILED(CVIBuffer::InitComponent_Proto()))
		return E_FAIL;

	////////////////////////////////////////////////////////////////////////
	// 버텍스에 좌표 입력

	// 버텍스 버퍼의 시작주소를 받아올 포인터 변수
	_VERTEX_TEX_NORMAL* pVertex = nullptr;
	
	// 버텍스 버퍼의 공간을 잠금
	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertex), 0);

	// 버텍스 정보 입력
	for(_uint i = 0; i < _uiVerticesCountZ; ++i){
		for(_uint j = 0; j < _uiVerticesCountX; ++j){
			_uint uiIndex = i * _uiVerticesCountX + j;

			pVertex[uiIndex].vPos = _vec3{j* _fInterval, 0.f, i*_fInterval };
			pVertex[uiIndex].vNormal = {0.f, 0.f, 0.f};
			pVertex[uiIndex].vTextureCoord = _vec2(j/ static_cast<float>(_uiVerticesCountX-1), i/static_cast<float>(_uiVerticesCountZ-1));
		}
	}

	////////////////////////////////////////////////////////////////////////
	// 인덱스 버퍼에 버텍스 버퍼 인덱스 입력

	// 인덱스 버퍼의 시작 주소를 받아올 포인터 변수
	INDEX32* pIndices = nullptr;

	// 인덱스 버퍼의 공간을 잠금
	m_pIndexBuffer->Lock(0,0,reinterpret_cast<void**>(&pIndices), 0);

	// 버텍스 인덱스로 이루어진 삼각형의 인덱스 변수
	_uint uiPolygonIndex = 0;

	// 인덱스 버퍼에 버텍스 버퍼의 인덱스 정보 입력
	for(_uint i = 0; i < _uiVerticesCountZ - 1; ++i){
		for(_uint j = 0; j < _uiVerticesCountX - 1; ++j){
			_uint uiIndex = i * _uiVerticesCountX + j;
	

			/* 삼각형 모양 예시: 가로크기 2, 세로 크기 2일때 */
			//	2-3				이런 형태로 삼각형을 만듦
			//	|\|				2, 3, 1이 첫번째 삼각형
			//	0-1				2, 1, 0이 두번째 삼각형

			// 첫번째 삼각형
			pIndices[uiPolygonIndex]._1 = uiIndex + _uiVerticesCountX;
			pIndices[uiPolygonIndex]._2 = uiIndex + _uiVerticesCountX + 1;
			pIndices[uiPolygonIndex]._3 = uiIndex + 1;

			m_pPolygonPositionArray[uiPolygonIndex]._1 = pVertex[pIndices[uiPolygonIndex]._1].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._2 = pVertex[pIndices[uiPolygonIndex]._2].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._3 = pVertex[pIndices[uiPolygonIndex]._3].vPos;

			// 정점의 범선벡터를 구하기 위한 폴리곤의 벡터 계산
			_vec3 vPolygonLine1 = pVertex[pIndices[uiPolygonIndex]._2].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;
			_vec3 vPolygonLine2 = pVertex[pIndices[uiPolygonIndex]._3].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;

			// 폴리곤을 이루는 두 벡터를 외적, 정규화
			_vec3 vNormal = {};
			D3DXVec3Cross(&vNormal, &vPolygonLine1, &vPolygonLine2);
			D3DXVec3Normalize(&vNormal, &vNormal);

			// 폴리곤을 이루는 각 3개의 정점에 구한 법선 벡터를 더해줌
			// 중복되어 사용되는 정점이 있기 때문에 더한뒤 후에 평균을 낸다.
			pVertex[pIndices[uiPolygonIndex]._1].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._2].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._3].vNormal += vNormal;

			++uiPolygonIndex;

			// 두번째 삼각형
			pIndices[uiPolygonIndex]._1 = uiIndex + _uiVerticesCountX;
			pIndices[uiPolygonIndex]._2 = uiIndex + 1;
			pIndices[uiPolygonIndex]._3 = uiIndex;

			m_pPolygonPositionArray[uiPolygonIndex]._1 = pVertex[pIndices[uiPolygonIndex]._1].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._2 = pVertex[pIndices[uiPolygonIndex]._2].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._3 = pVertex[pIndices[uiPolygonIndex]._3].vPos;


			// 정점의 범선벡터를 구하기 위한 폴리곤의 벡터 계산
			vPolygonLine1 = pVertex[pIndices[uiPolygonIndex]._2].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;
			vPolygonLine2 = pVertex[pIndices[uiPolygonIndex]._3].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;

			// 폴리곤을 이루는 두 벡터를 외적, 정규화
			D3DXVec3Cross(&vNormal, &vPolygonLine1, &vPolygonLine2);
			D3DXVec3Normalize(&vNormal, &vNormal);

			// 폴리곤을 이루는 각 3개의 정점에 구한 법선 벡터를 더해줌
			// 중복되어 사용되는 정점이 있기 때문에 더한뒤 후에 평균을 낸다.
			pVertex[pIndices[uiPolygonIndex]._1].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._2].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._3].vNormal += vNormal;

			++uiPolygonIndex;
		}
	}	

	// 버퍼 공간의 잠금 해제
	m_pVertexBuffer->Unlock();
	m_pIndexBuffer->Unlock();

	return NOERROR;
}

	// Initialize with Prototype(지형정보 BMP파일 경로)
HRESULT CVIBufferTerrain::InitComponent_Proto(const _tchar* _szFilePath, _float _fInterval/* = 1.f*/){
	if(nullptr == _szFilePath)
		return E_FAIL;

	// 파일 경로로부터 파일 읽어옴(BMP 파일)
	HANDLE hFile = CreateFile(_szFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_ulong dwByte = 0;

	// 비트맵 파일 헤더 구조체
	BITMAPFILEHEADER fh;
	// 비트맵 파일 헤더 구조체 만큼 읽어옴
	if(!ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr)){
		CloseHandle(hFile);
		return E_FAIL;
	}

	// 비트맵 정보 헤더 구조체
	BITMAPINFOHEADER ih;
	// 비트맵 정보 헤더 구조체 만큼 읽어옴
	if(!ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr)){
		CloseHandle(hFile);
		return E_FAIL;
	}

	//비트맵 정보 헤더에 들어있는 가로 세로 길이로 픽셀 배열 선언(한 픽셀당 32비트 혹은 24비트이기 때문에 32비트 변수로 선언)
	_ulong ulWidth = ih.biWidth, ulHeight = ih.biHeight;
	
	_ulong* pPixel = new _ulong[ulWidth * ulHeight];

	// 비트맵 이미지의 픽셀정보 얻어옴
	if(!ReadFile(hFile, pPixel, sizeof(_ulong) * ulWidth * ulHeight, &dwByte, nullptr)){
		CloseHandle(hFile);
		return E_FAIL;
	}

	////////////////////////////////////////////////////////////////////////

	// 지형 크기 정보 보관
	m_uiVerticesCountX = ulWidth;
	m_uiVerticesCountZ = ulHeight;
	m_fInterval = _fInterval;

	// 버텍스 구조체의 사이즈 설정
	m_uiStride = sizeof(_VERTEX_TEX_NORMAL);
	// 버텍스의 갯수 설정(비트맵의 가로 세로 길이만큼)
	m_uiVerticesCount = ulWidth * ulHeight;
	// FVF 설정
	m_ulFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	// 버텍스로 그려질 삼각형의 갯수 설정
	m_uiPrimitiveCount = (ulWidth - 1) * (ulHeight - 1) * 2;
	// 인덱스 버퍼 원소 3개의 사이즈
	m_uiPolygonIndicesSize = sizeof(INDEX32);
	// 폴리곤의 갯수만큼 버텍스 위치 정보를 보관하는 배열 할당
	m_pPolygonPositionArray = new POLYGON_POSITION[m_uiPrimitiveCount];
	// 인덱스 버퍼의 포맷 설정
	m_eFormat = D3DFMT_INDEX32;

	// 설정한 정보로 버텍스 버퍼와 인덱스 버퍼 할당
	if(FAILED(CVIBuffer::InitComponent_Proto()))
		return E_FAIL;

	////////////////////////////////////////////////////////////////////////
	// 버텍스에 좌표 입력

	// 버텍스 버퍼의 시작주소를 받아올 포인터 변수
	_VERTEX_TEX_NORMAL* pVertex = nullptr;

	// 버텍스 버퍼의 공간을 잠금
	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertex), 0);

	// 버텍스 정보 입력
	for(_uint i = 0; i < ulHeight; ++i){
		for(_uint j = 0; j < ulWidth; ++j){
			_uint uiIndex = i * ulWidth + j;

			pVertex[uiIndex].vPos = _vec3(j * _fInterval, (pPixel[uiIndex] & 0xff) * 0.2f, i * _fInterval);
			pVertex[uiIndex].vNormal = { 0.f, 0.f, 0.f };
			pVertex[uiIndex].vTextureCoord = _vec2(j / static_cast<float>(ulWidth - 1), i / static_cast<float>(ulHeight - 1));
		}
	}

	// 열었던 파일 핸들 닫음
	CloseHandle(hFile);

	////////////////////////////////////////////////////////////////////////
	// 인덱스 버퍼에 버텍스 버퍼 인덱스 입력

	// 인덱스 버퍼의 시작 주소를 받아올 포인터 변수
	INDEX32* pIndices = nullptr;

	// 인덱스 버퍼의 공간을 잠금
	m_pIndexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	// 버텍스 인덱스로 이루어진 삼각형의 인덱스 변수
	_uint uiPolygonIndex = 0;

	for(_uint i = 0; i < ulHeight - 1; ++i){
		for(_uint j = 0; j < ulWidth - 1; ++j){
			_uint uiIndex = i * ulWidth + j;

			/* 삼각형 모양 예시: 가로크기 2, 세로 크기 2일때 */
			//	2-3				이런 형태로 삼각형을 만듦
			//	|\|				2, 3, 1이 첫번째 삼각형
			//	0-1				2, 1, 0이 두번째 삼각형
			
			// 첫번째 삼각형
			pIndices[uiPolygonIndex]._1 = uiIndex + ulWidth;
			pIndices[uiPolygonIndex]._2 = uiIndex + ulWidth + 1;
			pIndices[uiPolygonIndex]._3 = uiIndex + 1;

			m_pPolygonPositionArray[uiPolygonIndex]._1 = pVertex[pIndices[uiPolygonIndex]._1].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._2 = pVertex[pIndices[uiPolygonIndex]._2].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._3 = pVertex[pIndices[uiPolygonIndex]._3].vPos;

			// 정점의 범선벡터를 구하기 위한 폴리곤의 벡터 계산
			_vec3 vPolygonLine1 = pVertex[pIndices[uiPolygonIndex]._2].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;
			_vec3 vPolygonLine2 = pVertex[pIndices[uiPolygonIndex]._3].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;

			// 폴리곤을 이루는 두 벡터를 외적, 정규화
			// 정규화를 하는 이유는 다른 법선 벡터들과 더하고 평균낼때 길이가 같아야 편하기 때문이다.
			_vec3 vNormal = {};
			D3DXVec3Cross(&vNormal, &vPolygonLine1, &vPolygonLine2);
			D3DXVec3Normalize(&vNormal, &vNormal);

			// 폴리곤을 이루는 각 3개의 정점에 구한 법선 벡터를 더해줌
			// 중복되어 사용되는 정점이 있기 때문에 더한뒤 후에 평균을 낸다.
			pVertex[pIndices[uiPolygonIndex]._1].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._2].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._3].vNormal += vNormal;
		
			++uiPolygonIndex;

			// 두번째 삼각형
			pIndices[uiPolygonIndex]._1 = uiIndex + ulWidth;
			pIndices[uiPolygonIndex]._2 = uiIndex + 1;
			pIndices[uiPolygonIndex]._3 = uiIndex;

			m_pPolygonPositionArray[uiPolygonIndex]._1 = pVertex[pIndices[uiPolygonIndex]._1].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._2 = pVertex[pIndices[uiPolygonIndex]._2].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._3 = pVertex[pIndices[uiPolygonIndex]._3].vPos;

			// 정점의 범선벡터를 구하기 위한 폴리곤의 벡터 계산
			vPolygonLine1 = pVertex[pIndices[uiPolygonIndex]._2].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;
			vPolygonLine2 = pVertex[pIndices[uiPolygonIndex]._3].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;

			// 폴리곤을 이루는 두 벡터를 외적, 정규화
			// 정규화를 하는 이유는 다른 법선 벡터들과 더하고 평균낼때 길이가 같아야 편하기 때문이다.
			D3DXVec3Cross(&vNormal, &vPolygonLine1, &vPolygonLine2);
			D3DXVec3Normalize(&vNormal, &vNormal);

			// 폴리곤을 이루는 각 3개의 정점에 구한 법선 벡터를 더해줌
			// 중복되어 사용되는 정점이 있기 때문에 더한뒤 후에 평균을 낸다.
			pVertex[pIndices[uiPolygonIndex]._1].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._2].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._3].vNormal += vNormal;

			++uiPolygonIndex;
		}
	}

	for(_uint i = 0; i < m_uiVerticesCount; ++i){
			// 각 정점의 법선벡터를 평균으로 만듬(더할때 정규화한 외적값을 더했으므로 정규화를 진행하면 평균과 같다)
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);
	}

	// 버퍼 공간의 잠금 해제
	m_pIndexBuffer->Unlock();
	m_pVertexBuffer->Unlock();

	SafeDelete_Array(pPixel);
	
	return NOERROR;
}

// Initialize with Clone
HRESULT CVIBufferTerrain::InitComponent_Clone(void* _pArgument){

	return NOERROR;
}

// Render
HRESULT CVIBufferTerrain::RenderVIBufferTerrain(){
	LPDIRECT3DDEVICE9 pGraphicDevice = GetGraphicDevice();
	if(nullptr == pGraphicDevice
		|| nullptr == m_pVertexBuffer
		|| nullptr == m_pIndexBuffer)
		return E_FAIL;

	SafeAddRef(pGraphicDevice);

	// 버텍스 버퍼 출력 준비
	pGraphicDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_uiStride);
	// FVF 설정
	pGraphicDevice->SetFVF(m_ulFVF);
	// 인덱스 버퍼 설정
	pGraphicDevice->SetIndices(m_pIndexBuffer);
	// 인덱스 버퍼를 통해 버텍스 참조하여 출력
	pGraphicDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uiVerticesCount, 0, m_uiPrimitiveCount);

	SafeRelease(pGraphicDevice);

	return NOERROR;
}

HRESULT CVIBufferTerrain::GetHeightMap(LPDIRECT3DTEXTURE9* _pTexture, _float _fHeightMax){
	if(nullptr == _pTexture)
		return E_FAIL;

	if(FAILED(D3DXCreateTexture(CComponent::GetGraphicDevice(), m_uiVerticesCountX, m_uiVerticesCountZ, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, _pTexture)))
		return E_FAIL;

	D3DLOCKED_RECT HeightRect;
	_VERTEX_TEX_NORMAL* pVertex = nullptr;
	(*_pTexture)->LockRect(0, &HeightRect, nullptr, 0);
	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertex), 0);

	_uint uiIndex = 0;

	for(_uint i = m_uiVerticesCountZ-1; i <= m_uiVerticesCountZ; --i)
		for(_uint j = 0; j < m_uiVerticesCountX; ++j){
			_uint uiVertexIndex = i * m_uiVerticesCountX + j;
			_float fHeight = pVertex[uiVertexIndex].vPos.y / _fHeightMax;

			reinterpret_cast<_ulong*>(HeightRect.pBits)[uiIndex++] = D3DXCOLOR(fHeight, fHeight, fHeight, fHeight);
		}

	m_pVertexBuffer->Unlock();
	(*_pTexture)->UnlockRect(0);

	return NOERROR;
}

// 버텍스의 위치정보를 갱신하는 함수(버텍스 수는 그대로)
HRESULT CVIBufferTerrain::SetVerticesPosition(POLYGON_POSITION * _pPolygonPosition, _uint _uiPrimitiveCount){
	if(nullptr == _pPolygonPosition
		|| m_uiPrimitiveCount != _uiPrimitiveCount
		|| nullptr == m_pVertexBuffer
		|| nullptr == m_pIndexBuffer)
		return E_FAIL;

	::memcpy(m_pPolygonPositionArray, _pPolygonPosition, sizeof(POLYGON_POSITION)*m_uiPrimitiveCount);

	_VERTEX_TEX_NORMAL* pVertex = nullptr;
	INDEX32* pIndices = nullptr;

	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertex), 0);
	m_pIndexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	// 버텍스 정보 입력
	for(_uint i = 0; i < m_uiPrimitiveCount; ++i){
		pVertex[pIndices[i]._1].vPos = m_pPolygonPositionArray[i]._1;
		pVertex[pIndices[i]._2].vPos = m_pPolygonPositionArray[i]._2;
		pVertex[pIndices[i]._3].vPos = m_pPolygonPositionArray[i]._3;
	}

	m_pIndexBuffer->Unlock();
	m_pVertexBuffer->Unlock();

	return NOERROR;
}

// 특정 버텍스의 위치정보를 갱신하는 함수
HRESULT CVIBufferTerrain::SetVerticesHeightPosition(const _vec3& _vPos, _uint _uiInner, _uint _uiOuter, _float _fHeight, _float _fHeightMax, _double _dDeltaTime, _bool _bIsContinuallyMode){
	if(nullptr == m_pVertexBuffer
		|| nullptr == m_pIndexBuffer)
		return E_FAIL;

	_VERTEX_TEX_NORMAL* pVertex = nullptr;
	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertex), 0);

	for(_int i= - static_cast<_int>(_uiOuter); i <= static_cast<_int>(_uiOuter); ++i)
		for(_int j = -static_cast<_int>(_uiOuter); j <= static_cast<_int>(_uiOuter); ++j){
			
			_int iPosZ = static_cast<_int>(round((_vPos.z - j) / m_fInterval));
			_int iPosX = static_cast<_int>(round((_vPos.x - i) / m_fInterval));
			
			if(0 > iPosZ || 0 > iPosX || iPosX >= static_cast<_int>(m_uiVerticesCountX) || iPosZ >= static_cast<_int>(m_uiVerticesCountZ))
				continue;

			_uint uiIndex = static_cast<_uint>(iPosZ) * m_uiVerticesCountX + static_cast<_uint>(iPosX);

			if(uiIndex >= m_uiVerticesCount)
				continue;

			if(!_bIsContinuallyMode){
				if((i >= -static_cast<_int>(_uiInner) && (i <= static_cast<_int>(_uiInner)))
					&& (j >= -static_cast<_int>(_uiInner) && j <= static_cast<_int>(_uiInner))){
					pVertex[uiIndex].vPos.y = _fHeight;
				} else{
					_int GradientIndex = (abs(i) - static_cast<_int>(_uiInner)) - (abs(j) - static_cast<_int>(_uiInner));

					_float RatioHeight = 0.f;
					if(0 > GradientIndex)
						RatioHeight = 1.f - (abs(j) - _uiInner) / static_cast<_float>(_uiOuter - _uiInner);
					else
						RatioHeight = 1.f - (abs(i) - _uiInner) / static_cast<_float>(_uiOuter - _uiInner);

					pVertex[uiIndex].vPos.y = (_fHeight * RatioHeight > pVertex[uiIndex].vPos.y) ? _fHeight * RatioHeight : pVertex[uiIndex].vPos.y;
				}
			} else{
				_fHeight = _fHeight < 0.1f ? -1.f : _fHeight;

				if((i >= -static_cast<_int>(_uiInner) && (i <= static_cast<_int>(_uiInner)))
					&& (j >= -static_cast<_int>(_uiInner) && j <= static_cast<_int>(_uiInner))){
					pVertex[uiIndex].vPos.y += _fHeight * static_cast<_float>(_dDeltaTime);
				} else{
					_int GradientIndex = (abs(i) - static_cast<_int>(_uiInner)) - (abs(j) - static_cast<_int>(_uiInner));

					_float RatioHeight = 0.f;
					if(0 > GradientIndex)
						RatioHeight = 1.f - (abs(j) - _uiInner) / static_cast<_float>(_uiOuter - _uiInner);
					else
						RatioHeight = 1.f - (abs(i) - _uiInner) / static_cast<_float>(_uiOuter - _uiInner);

					pVertex[uiIndex].vPos.y += _fHeight * RatioHeight * static_cast<_float>(_dDeltaTime);


				}
			}

			pVertex[uiIndex].vPos.y = pVertex[uiIndex].vPos.y > 50.f ? 50.f : pVertex[uiIndex].vPos.y;
		}

	INDEX32* pIndices;
	m_pIndexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	for(_uint i = 0; i < m_uiPrimitiveCount; ++i){
		m_pPolygonPositionArray[i]._1 = pVertex[pIndices[i]._1].vPos;
		m_pPolygonPositionArray[i]._2 = pVertex[pIndices[i]._2].vPos;
		m_pPolygonPositionArray[i]._3 = pVertex[pIndices[i]._3].vPos;
	}

	m_pIndexBuffer->Unlock();
	m_pVertexBuffer->Unlock();

	return NOERROR;
}

// 원본 생성 함수(static), 지형의 가로세로, 정점간격
CVIBufferTerrain* CVIBufferTerrain::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _uint _uiVerticesCountX, _uint _uiVerticesCountZ, _float _fIntaval){
	CVIBufferTerrain* pInstance = new CVIBufferTerrain(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_uiVerticesCountX, _uiVerticesCountZ, _fIntaval))){
		MSG_BOX(L"CVIBufferTerrain Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// 원본 생성 함수(static), 지형정보 BMP파일 경로, 정점간격
CVIBufferTerrain * CVIBufferTerrain::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szFilePath, _float _fInterval/* = 1.f*/){
	CVIBufferTerrain* pInstance = new CVIBufferTerrain(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_szFilePath))){
		MSG_BOX(L"CVIBufferTerrain Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
	
	return nullptr;
}

// 복제 생성 함수
CComponent * CVIBufferTerrain::CreateClone(void* _pArgument){
	CComponent* pInstance = new CVIBufferTerrain(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		MSG_BOX(L"CVIBufferTerrain Clone Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// Free
void CVIBufferTerrain::Free(){
	CVIBuffer::Free();
}
