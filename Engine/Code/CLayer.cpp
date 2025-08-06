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

	//해당 오브젝트 그룹의 첫 번째 요소를 가져옴
	auto iter = m_vecObject[(UINT)eObjId].begin();

	//TODO: 2025-08-03 다수의 오브젝트를 관리하는 상황에서 특정 오브젝트 찾는 로직 필요함
	// 현재는 플레이어 오브젝트를 찾는 용도로만 사용하는걸 가정하고있음
	
	return (*iter)->Get_Component(eID, pComponentTag);
}

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
		if (i == (_uint)OBJID::OBJ_PLAYER
			|| i == (_uint)OBJID::OBJ_PLAYERCAMERA)
			continue;

		DeleteObjectGroup((OBJID)i);
	}
}

void CLayer::Free()
{
	DeleteAll();
}
