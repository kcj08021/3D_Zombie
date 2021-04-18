#ifndef __BASE_H__
#define __BASE_H__

#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract{
protected:
	// 생성자
	explicit CBase();
	// 소멸자
	virtual ~CBase() = default;

public:
	// 레퍼런스 카운트 증가
	unsigned long AddRef();
	// Release
	unsigned long Release();

private:
	// 레퍼런스 카운트 변수
	unsigned long m_dwRefCount = 0;

public:
	//Free
	 virtual void Free() = 0;
};

END
#endif // !__BASE_H__
