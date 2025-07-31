#include "CLayer.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent* CLayer::Get_Component(COMPONENTID eID, OBJID eObjId, const _tchar* pComponentTag)
{
	if (m_vecObject[(UINT)eObjId].empty())
		return nullptr;

	auto iter = m_vecObject[(UINT)eObjId].begin();

	return (*iter)->Get_Component(eID, pComponentTag);
}

//CGameObject* CLayer::Get_Object(OBJID eObjId)
//{
//	vector<CGameObject*>::iterator iter;
//
//	auto idFinder = [&](vector<CGameObject*> vecToItem) { iter = find_if(vecToItem.begin(), vecToItem.end(), CID_Finder((_int)eObjId)); };
//
//	for_each(m_vecObject->begin(), m_vecObject->end(), idFinder);
//
//	if (iter == m_vecObject[(UINT)OBJID::OBJ_END].end());
//	return nullptr;
//
//	return *iter;
//}

HRESULT CLayer::Add_GameObject(OBJID eObjId, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecObject[(UINT)eObjId].push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float& fTimeDelta)
{
	_int iResult(0);

	for (size_t i = 0; i < (size_t)OBJID::OBJ_END; i++)
	{
		for (auto iter = m_vecObject[i].begin(); iter != m_vecObject[i].end(); )
		{
			iResult = (*iter)->Update_GameObject(fTimeDelta);

			if (iResult & 0x80000000)
				return iResult;

			++iter;
		}
	}

	return iResult;
}

void CLayer::LateUpdate_Layer(const _float& fTimeDelta)
{
	for (size_t i = 0; i < (size_t)OBJID::OBJ_END; i++)
	{
		for (auto iter = m_vecObject[i].begin(); iter != m_vecObject[i].end(); )
		{
			(*iter)->LateUpdate_GameObject(fTimeDelta);
			++iter;
		}
	}
}

CLayer* CLayer::Create()
{
	CLayer* pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer()))
	{
		MSG_BOX("Layer Create Failed");
		Safe_Release(pLayer);
		return nullptr;
	}

	return pLayer;
}

void CLayer::DeleteObjectGroup(OBJID _eTarget)
{
	auto& objList = m_vecObject[(UINT)_eTarget];

	_ulong dwRefCnt = 0;

	for (CGameObject* pObj : objList)
	{
		if (pObj != nullptr)
		{
			dwRefCnt = pObj->Release();

			if (0 == dwRefCnt)
				pObj = NULL;
		}
	}
	objList.clear();
}

void CLayer::DeleteAll()
{
	for (UINT i = 0; i < (UINT)OBJID::OBJ_END; ++i)
	{
		DeleteObjectGroup((OBJID)i);
	}
}

void CLayer::Free()
{
	DeleteAll();
}
