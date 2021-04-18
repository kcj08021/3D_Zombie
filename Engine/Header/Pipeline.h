#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CPipeline final:
	public CBase{
	// 싱글톤 정의
	DECLARE_SINGLETON(CPipeline)
private:
	// 생성자
	explicit CPipeline();
	// 소멸자
	virtual ~CPipeline() = default;

	// Get
public:
	// 변환 행렬을 얻어오는 함수
	HRESULT GetTransform(D3DTRANSFORMSTATETYPE _eTransform, _matrix* _pMatrix);
	const _matrix& GetCameraWorldMatrix(){
		return m_matCameraWorldMatrix;
	}
	// Set
public:
	// 변환 행렬을 설정 하는 함수
	HRESULT SetTransform(D3DTRANSFORMSTATETYPE _eTransform, const _matrix& _tMatirx);
	void SetCameraWorldMatrix(const _matrix& _matCameraWorldMatrix){
		m_matCameraWorldMatrix = _matCameraWorldMatrix;
	}

private:
	// 뷰 변환 행렬
	_matrix m_tViewMatrix;
	// 투영 행렬
	_matrix m_tProjectionMatrix;
	// 카메라의 월드 행렬
	_matrix m_matCameraWorldMatrix;

public:
	// Free
	virtual void Free() override;
};
END

#endif // !__PIPELINE_H__

