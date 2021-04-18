#ifndef __CLIENT_STRUCT_H__
#define __CLIENT_STRUCT_H__

#include "stdafx.h"

typedef struct tagStaticObjectSettingValue{
	float fScaleX = 1, fScaleY = 1, fScaleZ = 1;
	float fRotateX = 0, fRotateY = 0, fRotateZ = 0;
	float fPosX = 0, fPosY = 0, fPosZ = 0;
	wchar_t szProto[128] = L"";
	wchar_t szLayer[128] = L"";

	tagStaticObjectSettingValue& operator=(const tagStaticObjectSettingValue& _rhs){
		fScaleX = _rhs.fScaleX;
		fScaleY = _rhs.fScaleY;
		fScaleZ = _rhs.fScaleZ;
		fRotateX = _rhs.fRotateX;
		fRotateY = _rhs.fRotateY;
		fRotateZ = _rhs.fRotateZ;
		fPosX = _rhs.fPosX;
		fPosY = _rhs.fPosY;
		fPosZ = _rhs.fPosZ;
		lstrcpy(szProto, _rhs.szProto);
		lstrcpy(szLayer, _rhs.szLayer);

		return *this;
	}
}OBJECT_SETTING_INFO;

#endif // !__CLIENT_STRUCT_H__
