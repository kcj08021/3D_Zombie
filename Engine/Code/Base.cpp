#include "../Header/Base.h"

CBase::CBase(): m_dwRefCount(0){
}

// ���۷��� ī��Ʈ ����
unsigned long CBase::AddRef(){
	// ���۷��� ī��Ʈ�� ������ų�� ������Ų ���� ���� ��ȯ�Ѵ�.
	return ++m_dwRefCount;
}

// Release
unsigned long CBase::Release(){
	if(0 == m_dwRefCount){
		// ���۷��� ī��Ʈ�� 0�� ��� �ش� ��ü�� �Ҵ��� ������
		Free();

		delete this;

		return 0;
	} else{
		// ���۷��� ī��Ʈ�� ���ҽ�Ű�� ���� ���� ��ȯ
		return m_dwRefCount--;
	}
}
