#ifndef __ENGINE_MACRO_H__
#define __ENGINE_MACRO_H__

#ifdef ENGINE_EXPORTS
// ���� ���ο��� ����
#define ENGINE_DLL	_declspec(dllexport)
#else
// ���� �ܺο��� ����
#define ENGINE_DLL _declspec(dllimport)
#endif // ENGINE_EXPORTS

// namespace
#define BEGIN(NAMESPACE)	namespace NAMESPACE{
#define END					}
#define USING(NAMESPACE)	using namespace NAMESPACE;

// ���� ���� ����
#define NO_COPY(CLASS_NAME)						\
private:										\
	explicit CLASS_NAME(const CLASS_NAME&);		\
	CLASS_NAME& operator=(const CLASS_NAME&);

// �̱��� ���Ǻ�
#define DECLARE_SINGLETON(CLASS_NAME)           \
NO_COPY(CLASS_NAME);							\
private:                                        \
    static CLASS_NAME* pInstance;               \
public:                                         \
    static CLASS_NAME* GetInstance();           \
    unsigned long DestroyInstance();


// �̱��� ������
#define IMPLEMENT_SINGLETON(CLASS_NAME)         \
CLASS_NAME* CLASS_NAME::pInstance = nullptr;    \
                                                \
CLASS_NAME* CLASS_NAME::GetInstance(){          \
    if(nullptr == pInstance)                    \
        pInstance = new CLASS_NAME;             \
                                                \
    return pInstance;                           \
}                                               \
                                                \
unsigned long CLASS_NAME::DestroyInstance(){    \
	unsigned long dwRefCount = 0;				\
    if(nullptr != pInstance){                   \
		dwRefCount = pInstance->Release();		\
		if(0 == dwRefCount)						\
			 pInstance = nullptr;               \
    }                                           \
	return dwRefCount;							\
}

// MessageBox
#define MSG_BOX(MESSAGE) ::MessageBox(0, MESSAGE, L"Error", MB_OK)


#endif // !__ENGINE_MACRO_H__
