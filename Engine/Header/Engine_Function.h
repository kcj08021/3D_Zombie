#ifndef __ENGINE_FUNCTION_H__
#define __ENGINE_FUNCTION_H__

namespace Engine{
	// �Ϲ������� ������ ��ü�� ����
	template <typename T>
	void SafeDelete(T& _pInstance){
		if(nullptr != _pInstance){
			delete _pInstance;
			_pInstance = nullptr;
		}
	}


	// �Ϲ������� ������ �迭 ��ü�� ����
	template <typename T>
	void SafeDelete_Array(T& _pInstance){
		if(nullptr != _pInstance){
			delete[] _pInstance;
			_pInstance = nullptr;
		}
	}

	// ���۷��� ī���ͷ� �����Ǵ� ��ü�� ���۷��� ī���͸� ����.
	template <typename T>
	unsigned long SafeAddRef(T& _pInstance){
		unsigned long dwRefCount = 0;
		if(nullptr != _pInstance)
			dwRefCount = _pInstance->AddRef();

		return dwRefCount;
	}

	// ���۷��� ī��Ʈ�� �����ǰ� �ִ� ��ü�� �����ϰ��� �Ҷ�. 
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
