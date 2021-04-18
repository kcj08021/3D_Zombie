#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CPipeline final:
	public CBase{
	// �̱��� ����
	DECLARE_SINGLETON(CPipeline)
private:
	// ������
	explicit CPipeline();
	// �Ҹ���
	virtual ~CPipeline() = default;

	// Get
public:
	// ��ȯ ����� ������ �Լ�
	HRESULT GetTransform(D3DTRANSFORMSTATETYPE _eTransform, _matrix* _pMatrix);
	const _matrix& GetCameraWorldMatrix(){
		return m_matCameraWorldMatrix;
	}
	// Set
public:
	// ��ȯ ����� ���� �ϴ� �Լ�
	HRESULT SetTransform(D3DTRANSFORMSTATETYPE _eTransform, const _matrix& _tMatirx);
	void SetCameraWorldMatrix(const _matrix& _matCameraWorldMatrix){
		m_matCameraWorldMatrix = _matCameraWorldMatrix;
	}

private:
	// �� ��ȯ ���
	_matrix m_tViewMatrix;
	// ���� ���
	_matrix m_tProjectionMatrix;
	// ī�޶��� ���� ���
	_matrix m_matCameraWorldMatrix;

public:
	// Free
	virtual void Free() override;
};
END

#endif // !__PIPELINE_H__

