#include "../Header/Base.h"

CBase::CBase(): m_dwRefCount(0){
}

// 레퍼런스 카운트 증가
unsigned long CBase::AddRef(){
	// 레퍼런스 카운트를 증가시킬땐 증가시킨 뒤의 값을 반환한다.
	return ++m_dwRefCount;
}

// Release
unsigned long CBase::Release(){
	if(0 == m_dwRefCount){
		// 레퍼런스 카운트가 0인 경우 해당 객체의 할당을 해제함
		Free();

		delete this;

		return 0;
	} else{
		// 레퍼런스 카운트를 감소시키기 전의 값을 반환
		return m_dwRefCount--;
	}
}
