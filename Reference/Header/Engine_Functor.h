#ifndef __ENGINE_FUNCTOR_H__
#define __ENGINE_FUNCTOR_H__

// ���ڿ��� ���Է� �޾� pair ��ü�� first�� ������ �� �� bool�� ��ȯ�ϴ� �Լ� ��ü
class IsEqual_tchar{
public:
	IsEqual_tchar(const wchar_t* _szFindString):m_szFindString(_szFindString){}

private:
	const wchar_t* m_szFindString = nullptr;

public:
	template<typename T>
	bool operator()(T& _mapPair){
		return !lstrcmp(_mapPair.first, m_szFindString);
	}
};

class IsEqual_Address{
public:
	IsEqual_Address(const void* _szFindAddress):m_szFindAddress(_szFindAddress){}

private:
	const void* m_szFindAddress = nullptr;

public:
	bool operator()(void * _pParam){
		return _pParam == m_szFindAddress;
	}
};

#endif // !__ENGINE_FUNCTOR_H__
