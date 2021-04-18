#include "..\Header\VIBufferTerrain.h"

// ������
CVIBufferTerrain::CVIBufferTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CVIBuffer(_pGraphicDevice)
	, m_uiVerticesCountX(0)
	, m_uiVerticesCountZ(0)
	, m_fInterval(0.f){
}

// ���� ������
CVIBufferTerrain::CVIBufferTerrain(const CVIBufferTerrain & _VIBufferTerrain)
	: CVIBuffer(_VIBufferTerrain)
	, m_uiVerticesCountX(_VIBufferTerrain.m_uiVerticesCountX)
	, m_uiVerticesCountZ(_VIBufferTerrain.m_uiVerticesCountZ)
	, m_fInterval(_VIBufferTerrain.m_fInterval){
}

// Initialize with Prototype(���� ���μ��� ũ��)
HRESULT CVIBufferTerrain::InitComponent_Proto(_uint _uiVerticesCountX, _uint _uiVerticesCountZ, _float _fInterval/* = 1.f*/){
	if(1 >= _uiVerticesCountX
		|| 1 >= _uiVerticesCountZ
		|| 0.1 >= _fInterval)
		return E_FAIL;

	// ���� ũ�� ���� ����
	m_uiVerticesCountX = _uiVerticesCountX;
	m_uiVerticesCountZ = _uiVerticesCountZ;
	m_fInterval = _fInterval;
	
	// ���ؽ� ����ü�� ������ ����
	m_uiStride = sizeof(_VERTEX_TEX_NORMAL);
	// ���ؽ��� ���� ����
	m_uiVerticesCount = _uiVerticesCountX * _uiVerticesCountZ;
	// FVF ����
	m_ulFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	// ���ؽ��� �׷��� �ﰢ���� ���� ����
	m_uiPrimitiveCount = (_uiVerticesCountX - 1) * (_uiVerticesCountZ - 1) * 2;

	// �������� ������ŭ ���ؽ� ��ġ ������ �����ϴ� �迭 �Ҵ�
	m_pPolygonPositionArray = new POLYGON_POSITION[m_uiPrimitiveCount];

	// �ε��� ���� ���� 3���� ������
	m_uiPolygonIndicesSize = sizeof(INDEX32);
	// �ε��� ������ ���� ����
	m_eFormat = D3DFMT_INDEX32;

	// ������ ������ ���ؽ� ���ۿ� �ε��� ���� �Ҵ�
	if(FAILED(CVIBuffer::InitComponent_Proto()))
		return E_FAIL;

	////////////////////////////////////////////////////////////////////////
	// ���ؽ��� ��ǥ �Է�

	// ���ؽ� ������ �����ּҸ� �޾ƿ� ������ ����
	_VERTEX_TEX_NORMAL* pVertex = nullptr;
	
	// ���ؽ� ������ ������ ���
	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertex), 0);

	// ���ؽ� ���� �Է�
	for(_uint i = 0; i < _uiVerticesCountZ; ++i){
		for(_uint j = 0; j < _uiVerticesCountX; ++j){
			_uint uiIndex = i * _uiVerticesCountX + j;

			pVertex[uiIndex].vPos = _vec3{j* _fInterval, 0.f, i*_fInterval };
			pVertex[uiIndex].vNormal = {0.f, 0.f, 0.f};
			pVertex[uiIndex].vTextureCoord = _vec2(j/ static_cast<float>(_uiVerticesCountX-1), i/static_cast<float>(_uiVerticesCountZ-1));
		}
	}

	////////////////////////////////////////////////////////////////////////
	// �ε��� ���ۿ� ���ؽ� ���� �ε��� �Է�

	// �ε��� ������ ���� �ּҸ� �޾ƿ� ������ ����
	INDEX32* pIndices = nullptr;

	// �ε��� ������ ������ ���
	m_pIndexBuffer->Lock(0,0,reinterpret_cast<void**>(&pIndices), 0);

	// ���ؽ� �ε����� �̷���� �ﰢ���� �ε��� ����
	_uint uiPolygonIndex = 0;

	// �ε��� ���ۿ� ���ؽ� ������ �ε��� ���� �Է�
	for(_uint i = 0; i < _uiVerticesCountZ - 1; ++i){
		for(_uint j = 0; j < _uiVerticesCountX - 1; ++j){
			_uint uiIndex = i * _uiVerticesCountX + j;
	

			/* �ﰢ�� ��� ����: ����ũ�� 2, ���� ũ�� 2�϶� */
			//	2-3				�̷� ���·� �ﰢ���� ����
			//	|\|				2, 3, 1�� ù��° �ﰢ��
			//	0-1				2, 1, 0�� �ι�° �ﰢ��

			// ù��° �ﰢ��
			pIndices[uiPolygonIndex]._1 = uiIndex + _uiVerticesCountX;
			pIndices[uiPolygonIndex]._2 = uiIndex + _uiVerticesCountX + 1;
			pIndices[uiPolygonIndex]._3 = uiIndex + 1;

			m_pPolygonPositionArray[uiPolygonIndex]._1 = pVertex[pIndices[uiPolygonIndex]._1].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._2 = pVertex[pIndices[uiPolygonIndex]._2].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._3 = pVertex[pIndices[uiPolygonIndex]._3].vPos;

			// ������ �������͸� ���ϱ� ���� �������� ���� ���
			_vec3 vPolygonLine1 = pVertex[pIndices[uiPolygonIndex]._2].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;
			_vec3 vPolygonLine2 = pVertex[pIndices[uiPolygonIndex]._3].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;

			// �������� �̷�� �� ���͸� ����, ����ȭ
			_vec3 vNormal = {};
			D3DXVec3Cross(&vNormal, &vPolygonLine1, &vPolygonLine2);
			D3DXVec3Normalize(&vNormal, &vNormal);

			// �������� �̷�� �� 3���� ������ ���� ���� ���͸� ������
			// �ߺ��Ǿ� ���Ǵ� ������ �ֱ� ������ ���ѵ� �Ŀ� ����� ����.
			pVertex[pIndices[uiPolygonIndex]._1].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._2].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._3].vNormal += vNormal;

			++uiPolygonIndex;

			// �ι�° �ﰢ��
			pIndices[uiPolygonIndex]._1 = uiIndex + _uiVerticesCountX;
			pIndices[uiPolygonIndex]._2 = uiIndex + 1;
			pIndices[uiPolygonIndex]._3 = uiIndex;

			m_pPolygonPositionArray[uiPolygonIndex]._1 = pVertex[pIndices[uiPolygonIndex]._1].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._2 = pVertex[pIndices[uiPolygonIndex]._2].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._3 = pVertex[pIndices[uiPolygonIndex]._3].vPos;


			// ������ �������͸� ���ϱ� ���� �������� ���� ���
			vPolygonLine1 = pVertex[pIndices[uiPolygonIndex]._2].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;
			vPolygonLine2 = pVertex[pIndices[uiPolygonIndex]._3].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;

			// �������� �̷�� �� ���͸� ����, ����ȭ
			D3DXVec3Cross(&vNormal, &vPolygonLine1, &vPolygonLine2);
			D3DXVec3Normalize(&vNormal, &vNormal);

			// �������� �̷�� �� 3���� ������ ���� ���� ���͸� ������
			// �ߺ��Ǿ� ���Ǵ� ������ �ֱ� ������ ���ѵ� �Ŀ� ����� ����.
			pVertex[pIndices[uiPolygonIndex]._1].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._2].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._3].vNormal += vNormal;

			++uiPolygonIndex;
		}
	}	

	// ���� ������ ��� ����
	m_pVertexBuffer->Unlock();
	m_pIndexBuffer->Unlock();

	return NOERROR;
}

	// Initialize with Prototype(�������� BMP���� ���)
HRESULT CVIBufferTerrain::InitComponent_Proto(const _tchar* _szFilePath, _float _fInterval/* = 1.f*/){
	if(nullptr == _szFilePath)
		return E_FAIL;

	// ���� ��ηκ��� ���� �о��(BMP ����)
	HANDLE hFile = CreateFile(_szFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_ulong dwByte = 0;

	// ��Ʈ�� ���� ��� ����ü
	BITMAPFILEHEADER fh;
	// ��Ʈ�� ���� ��� ����ü ��ŭ �о��
	if(!ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr)){
		CloseHandle(hFile);
		return E_FAIL;
	}

	// ��Ʈ�� ���� ��� ����ü
	BITMAPINFOHEADER ih;
	// ��Ʈ�� ���� ��� ����ü ��ŭ �о��
	if(!ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr)){
		CloseHandle(hFile);
		return E_FAIL;
	}

	//��Ʈ�� ���� ����� ����ִ� ���� ���� ���̷� �ȼ� �迭 ����(�� �ȼ��� 32��Ʈ Ȥ�� 24��Ʈ�̱� ������ 32��Ʈ ������ ����)
	_ulong ulWidth = ih.biWidth, ulHeight = ih.biHeight;
	
	_ulong* pPixel = new _ulong[ulWidth * ulHeight];

	// ��Ʈ�� �̹����� �ȼ����� ����
	if(!ReadFile(hFile, pPixel, sizeof(_ulong) * ulWidth * ulHeight, &dwByte, nullptr)){
		CloseHandle(hFile);
		return E_FAIL;
	}

	////////////////////////////////////////////////////////////////////////

	// ���� ũ�� ���� ����
	m_uiVerticesCountX = ulWidth;
	m_uiVerticesCountZ = ulHeight;
	m_fInterval = _fInterval;

	// ���ؽ� ����ü�� ������ ����
	m_uiStride = sizeof(_VERTEX_TEX_NORMAL);
	// ���ؽ��� ���� ����(��Ʈ���� ���� ���� ���̸�ŭ)
	m_uiVerticesCount = ulWidth * ulHeight;
	// FVF ����
	m_ulFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	// ���ؽ��� �׷��� �ﰢ���� ���� ����
	m_uiPrimitiveCount = (ulWidth - 1) * (ulHeight - 1) * 2;
	// �ε��� ���� ���� 3���� ������
	m_uiPolygonIndicesSize = sizeof(INDEX32);
	// �������� ������ŭ ���ؽ� ��ġ ������ �����ϴ� �迭 �Ҵ�
	m_pPolygonPositionArray = new POLYGON_POSITION[m_uiPrimitiveCount];
	// �ε��� ������ ���� ����
	m_eFormat = D3DFMT_INDEX32;

	// ������ ������ ���ؽ� ���ۿ� �ε��� ���� �Ҵ�
	if(FAILED(CVIBuffer::InitComponent_Proto()))
		return E_FAIL;

	////////////////////////////////////////////////////////////////////////
	// ���ؽ��� ��ǥ �Է�

	// ���ؽ� ������ �����ּҸ� �޾ƿ� ������ ����
	_VERTEX_TEX_NORMAL* pVertex = nullptr;

	// ���ؽ� ������ ������ ���
	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertex), 0);

	// ���ؽ� ���� �Է�
	for(_uint i = 0; i < ulHeight; ++i){
		for(_uint j = 0; j < ulWidth; ++j){
			_uint uiIndex = i * ulWidth + j;

			pVertex[uiIndex].vPos = _vec3(j * _fInterval, (pPixel[uiIndex] & 0xff) * 0.2f, i * _fInterval);
			pVertex[uiIndex].vNormal = { 0.f, 0.f, 0.f };
			pVertex[uiIndex].vTextureCoord = _vec2(j / static_cast<float>(ulWidth - 1), i / static_cast<float>(ulHeight - 1));
		}
	}

	// ������ ���� �ڵ� ����
	CloseHandle(hFile);

	////////////////////////////////////////////////////////////////////////
	// �ε��� ���ۿ� ���ؽ� ���� �ε��� �Է�

	// �ε��� ������ ���� �ּҸ� �޾ƿ� ������ ����
	INDEX32* pIndices = nullptr;

	// �ε��� ������ ������ ���
	m_pIndexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	// ���ؽ� �ε����� �̷���� �ﰢ���� �ε��� ����
	_uint uiPolygonIndex = 0;

	for(_uint i = 0; i < ulHeight - 1; ++i){
		for(_uint j = 0; j < ulWidth - 1; ++j){
			_uint uiIndex = i * ulWidth + j;

			/* �ﰢ�� ��� ����: ����ũ�� 2, ���� ũ�� 2�϶� */
			//	2-3				�̷� ���·� �ﰢ���� ����
			//	|\|				2, 3, 1�� ù��° �ﰢ��
			//	0-1				2, 1, 0�� �ι�° �ﰢ��
			
			// ù��° �ﰢ��
			pIndices[uiPolygonIndex]._1 = uiIndex + ulWidth;
			pIndices[uiPolygonIndex]._2 = uiIndex + ulWidth + 1;
			pIndices[uiPolygonIndex]._3 = uiIndex + 1;

			m_pPolygonPositionArray[uiPolygonIndex]._1 = pVertex[pIndices[uiPolygonIndex]._1].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._2 = pVertex[pIndices[uiPolygonIndex]._2].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._3 = pVertex[pIndices[uiPolygonIndex]._3].vPos;

			// ������ �������͸� ���ϱ� ���� �������� ���� ���
			_vec3 vPolygonLine1 = pVertex[pIndices[uiPolygonIndex]._2].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;
			_vec3 vPolygonLine2 = pVertex[pIndices[uiPolygonIndex]._3].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;

			// �������� �̷�� �� ���͸� ����, ����ȭ
			// ����ȭ�� �ϴ� ������ �ٸ� ���� ���͵�� ���ϰ� ��ճ��� ���̰� ���ƾ� ���ϱ� �����̴�.
			_vec3 vNormal = {};
			D3DXVec3Cross(&vNormal, &vPolygonLine1, &vPolygonLine2);
			D3DXVec3Normalize(&vNormal, &vNormal);

			// �������� �̷�� �� 3���� ������ ���� ���� ���͸� ������
			// �ߺ��Ǿ� ���Ǵ� ������ �ֱ� ������ ���ѵ� �Ŀ� ����� ����.
			pVertex[pIndices[uiPolygonIndex]._1].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._2].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._3].vNormal += vNormal;
		
			++uiPolygonIndex;

			// �ι�° �ﰢ��
			pIndices[uiPolygonIndex]._1 = uiIndex + ulWidth;
			pIndices[uiPolygonIndex]._2 = uiIndex + 1;
			pIndices[uiPolygonIndex]._3 = uiIndex;

			m_pPolygonPositionArray[uiPolygonIndex]._1 = pVertex[pIndices[uiPolygonIndex]._1].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._2 = pVertex[pIndices[uiPolygonIndex]._2].vPos;
			m_pPolygonPositionArray[uiPolygonIndex]._3 = pVertex[pIndices[uiPolygonIndex]._3].vPos;

			// ������ �������͸� ���ϱ� ���� �������� ���� ���
			vPolygonLine1 = pVertex[pIndices[uiPolygonIndex]._2].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;
			vPolygonLine2 = pVertex[pIndices[uiPolygonIndex]._3].vPos - pVertex[pIndices[uiPolygonIndex]._1].vPos;

			// �������� �̷�� �� ���͸� ����, ����ȭ
			// ����ȭ�� �ϴ� ������ �ٸ� ���� ���͵�� ���ϰ� ��ճ��� ���̰� ���ƾ� ���ϱ� �����̴�.
			D3DXVec3Cross(&vNormal, &vPolygonLine1, &vPolygonLine2);
			D3DXVec3Normalize(&vNormal, &vNormal);

			// �������� �̷�� �� 3���� ������ ���� ���� ���͸� ������
			// �ߺ��Ǿ� ���Ǵ� ������ �ֱ� ������ ���ѵ� �Ŀ� ����� ����.
			pVertex[pIndices[uiPolygonIndex]._1].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._2].vNormal += vNormal;
			pVertex[pIndices[uiPolygonIndex]._3].vNormal += vNormal;

			++uiPolygonIndex;
		}
	}

	for(_uint i = 0; i < m_uiVerticesCount; ++i){
			// �� ������ �������͸� ������� ����(���Ҷ� ����ȭ�� �������� �������Ƿ� ����ȭ�� �����ϸ� ��հ� ����)
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);
	}

	// ���� ������ ��� ����
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

	// ���ؽ� ���� ��� �غ�
	pGraphicDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_uiStride);
	// FVF ����
	pGraphicDevice->SetFVF(m_ulFVF);
	// �ε��� ���� ����
	pGraphicDevice->SetIndices(m_pIndexBuffer);
	// �ε��� ���۸� ���� ���ؽ� �����Ͽ� ���
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

// ���ؽ��� ��ġ������ �����ϴ� �Լ�(���ؽ� ���� �״��)
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

	// ���ؽ� ���� �Է�
	for(_uint i = 0; i < m_uiPrimitiveCount; ++i){
		pVertex[pIndices[i]._1].vPos = m_pPolygonPositionArray[i]._1;
		pVertex[pIndices[i]._2].vPos = m_pPolygonPositionArray[i]._2;
		pVertex[pIndices[i]._3].vPos = m_pPolygonPositionArray[i]._3;
	}

	m_pIndexBuffer->Unlock();
	m_pVertexBuffer->Unlock();

	return NOERROR;
}

// Ư�� ���ؽ��� ��ġ������ �����ϴ� �Լ�
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

// ���� ���� �Լ�(static), ������ ���μ���, ��������
CVIBufferTerrain* CVIBufferTerrain::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _uint _uiVerticesCountX, _uint _uiVerticesCountZ, _float _fIntaval){
	CVIBufferTerrain* pInstance = new CVIBufferTerrain(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_uiVerticesCountX, _uiVerticesCountZ, _fIntaval))){
		MSG_BOX(L"CVIBufferTerrain Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// ���� ���� �Լ�(static), �������� BMP���� ���, ��������
CVIBufferTerrain * CVIBufferTerrain::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szFilePath, _float _fInterval/* = 1.f*/){
	CVIBufferTerrain* pInstance = new CVIBufferTerrain(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_szFilePath))){
		MSG_BOX(L"CVIBufferTerrain Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
	
	return nullptr;
}

// ���� ���� �Լ�
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
