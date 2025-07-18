#include "CPrototypeManager.h"

IMPLEMENT_SINGLETON(CPrototypeManager)

CPrototypeManager::CPrototypeManager()
{
}

CPrototypeManager::~CPrototypeManager()
{
	Free();
}

HRESULT CPrototypeManager::Ready_Prototype(const _tchar* pComponentTag, CComponent* pComponent)
{
	CComponent* pInstance = Find_Prototype(pComponentTag);
	if (nullptr != pInstance)
		return E_FAIL;

	m_mapPrototype.insert({ pComponentTag, pComponent });

	return S_OK;
}

CComponent* CPrototypeManager::Clone_Prototype(const _tchar* pComponentTag)
{
	CComponent* pInstance = Find_Prototype(pComponentTag);
	if (nullptr == pInstance)
		return nullptr;

	return pInstance->Clone();
}

CComponent* CPrototypeManager::Find_Prototype(const _tchar* pComponentTag)
{
	auto iter = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CTag_Finder(pComponentTag));

	if (iter == m_mapPrototype.end())
		return nullptr;

	return iter->second;
}

void CPrototypeManager::Free()
{
	for_each(m_mapPrototype.begin(), m_mapPrototype.end(), CDeleteMap());
	m_mapPrototype.clear();
}
