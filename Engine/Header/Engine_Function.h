#ifndef __ENGINE_FUNCTION_H__
#define __ENGINE_FUNCTION_H__

namespace Engine{
	// 일반적으로 생성된 객체의 삭제
	template <typename T>
	void SafeDelete(T& _pInstance){
		if(nullptr != _pInstance){
			delete _pInstance;
			_pInstance = nullptr;
		}
	}


	// 일반적으로 생성된 배열 객체의 삭제
	template <typename T>
	void SafeDelete_Array(T& _pInstance){
		if(nullptr != _pInstance){
			delete[] _pInstance;
			_pInstance = nullptr;
		}
	}

	// 레퍼런스 카운터로 관리되는 객체의 레퍼런스 카운터를 증가.
	template <typename T>
	unsigned long SafeAddRef(T& _pInstance){
		unsigned long dwRefCount = 0;
		if(nullptr != _pInstance)
			dwRefCount = _pInstance->AddRef();

		return dwRefCount;
	}

	// 레퍼런스 카운트로 관리되고 있는 객체를 삭제하고자 할때. 
	template <typename T>
	unsigned long SafeRelease(T& _pInstance){
		unsigned long dwRefCount = 0;
		if(nullptr != _pInstance){
			dwRefCount = _pInstance->Release();
			if(0 == dwRefCount)
				_pInstance = nullptr;
		}
		return dwRefCount;
	}
}

#endif // !__ENGINE_FUNCTION_H__
