#ifndef __ENGINE_MACRO_H__
#define __ENGINE_MACRO_H__

#ifdef ENGINE_EXPORTS
// 엔진 내부에서 접근
#define ENGINE_DLL	_declspec(dllexport)
#else
// 엔진 외부에서 접근
#define ENGINE_DLL _declspec(dllimport)
#endif // ENGINE_EXPORTS

// namespace
#define BEGIN(NAMESPACE)	namespace NAMESPACE{
#define END					}
#define USING(NAMESPACE)	using namespace NAMESPACE;

// 복사 생성 금지
#define NO_COPY(CLASS_NAME)						\
private:										\
	explicit CLASS_NAME(const CLASS_NAME&);		\
	CLASS_NAME& operator=(const CLASS_NAME&);

// 싱글톤 정의부
#define DECLARE_SINGLETON(CLASS_NAME)           \
NO_COPY(CLASS_NAME);							\
private:                                        \
    static CLASS_NAME* pInstance;               \
public:                                         \
    static CLASS_NAME* GetInstance();           \
    unsigned long DestroyInstance();


// 싱글톤 구현부
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
