#include "CObjectFactory.h"
IMPLEMENT_SINGLETON(CObjectFactory)

CObjectFactory::CObjectFactory()
{
}

CObjectFactory::~CObjectFactory()
{
	Free();
}

CGameObject* CObjectFactory::Create_Object(CGameObject* pObj, OBJID eObjId, _vec3 vPos, _tchar* texTag)
{
	return nullptr;
}

CGameObject* CObjectFactory::Create_Terrain(CGameObject* pObj, OBJID eObjId, _vec3 vPos, _vec3 vScale, _tchar* texTag)
{
	return nullptr;
}

void CObjectFactory::Free()
{
}
