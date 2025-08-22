#include "CPrototypeManager.h"

IMPLEMENT_SINGLETON(CPrototypeManager)

CPrototypeManager::CPrototypeManager()
{
}

CPrototypeManager::~CPrototypeManager()
{
	Free();
}

HRESULT CPrototypeManager::Ready_ComPrototype(wstring strComponentTag, CComponent* pComponent)
{
	CComponent* pInstance = Find_ComPrototype(strComponentTag);
	if (nullptr != pInstance)
		return E_FAIL;

	m_mapComPrototype.insert({ strComponentTag, pComponent });

	return S_OK;
}

CComponent* CPrototypeManager::Clone_ComPrototype(wstring strComponentTag)
{
	CComponent* pInstance = Find_ComPrototype(strComponentTag);
	if (nullptr == pInstance)
		return nullptr;

	return pInstance->Clone();
}

HRESULT CPrototypeManager::Ready_ObjPrototype(OBJPROTOTYPE eObjId, CGameObject* pComponent)
{
	CGameObject* pInstance = Find_ObjPrototype(eObjId);
	if (nullptr != pInstance)
		return E_FAIL;

	m_mapObjPrototype.insert({ eObjId, pComponent });

	return S_OK;
}

CGameObject* CPrototypeManager::Clone_ObjPrototype(OBJPROTOTYPE eObjId)
{
	CGameObject* pInstance = Find_ObjPrototype(eObjId);

	if (nullptr == pInstance)
		return nullptr;

	return pInstance->Clone();
}

CGameObject* CPrototypeManager::Clone_ObjPrototype(OBJPROTOTYPE eObjId, CGameObject* _Owner)
{
	CGameObject* pInstance = Find_ObjPrototype(eObjId);

	if (nullptr == pInstance)
		return nullptr;

	return pInstance->Clone(_Owner);
}

CComponent* CPrototypeManager::Find_ComPrototype(wstring pComponentTag)
{
	auto iter = find_if(m_mapComPrototype.begin(), m_mapComPrototype.end(), [&pComponentTag](const pair<wstring, CComponent*> pair) { return pair.first == pComponentTag; });

	if (iter == m_mapComPrototype.end())
		return nullptr;

	return iter->second;
}

CGameObject* CPrototypeManager::Find_ObjPrototype(OBJPROTOTYPE eObjId)
{
	auto iter = find_if(m_mapObjPrototype.begin(), m_mapObjPrototype.end(), [&eObjId](const pair<const OBJPROTOTYPE, CGameObject*> pair) { return pair.first == eObjId; });

	if (iter == m_mapObjPrototype.end())
		return nullptr;

	return iter->second;
}

void CPrototypeManager::Free()
{
	for_each(m_mapComPrototype.begin(), m_mapComPrototype.end(), CDeleteMap());
	m_mapComPrototype.clear();

	for_each(m_mapObjPrototype.begin(), m_mapObjPrototype.end(), CDeleteMap());
	m_mapObjPrototype.clear();
}
