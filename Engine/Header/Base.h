#ifndef __BASE_H__
#define __BASE_H__

#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract{
protected:
	// ������
	explicit CBase();
	// �Ҹ���
	virtual ~CBase() = default;

public:
	// ���۷��� ī��Ʈ ����
	unsigned long AddRef();
	// Release
	unsigned long Release();

private:
	// ���۷��� ī��Ʈ ����
	unsigned long m_dwRefCount = 0;

public:
	//Free
	 virtual void Free() = 0;
};

END
#endif // !__BASE_H__
