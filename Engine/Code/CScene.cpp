#include "CScene.h"

CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CScene::~CScene()
{
}

CComponent* CScene::Get_Component(COMPONENTID eID, LAYERID eLayerId, OBJID eObjId, const _tchar* pComponentTag)
{
	auto iter = find_if(m_mapLayer.begin(),
		m_mapLayer.end(),
		[eLayerId](const pair<const LAYERID, const CLayer*> pair) { return pair.first == eLayerId; });

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_Component(eID, eObjId, pComponentTag);
}

const vector<CGameObject*>& CScene::Get_ObjectGroup(LAYERID eLayerId, OBJID eObjId)
{
	vector<CGameObject*> result;

	auto iter = find_if(m_mapLayer.begin(),
		m_mapLayer.end(),
		[eLayerId](const pair<const LAYERID, const CLayer*> pair) { return pair.first == eLayerId; });

	if (iter == m_mapLayer.end())
		return result;

	return iter->second->Get_ObjectGroup(eObjId);
}

const vector<CGameObject*> CScene::Get_TotalGameObject()
{
	vector<CGameObject*> result;
	for (auto layer : m_mapLayer)
	{
		auto objArrList = layer.second->Get_AllObject();
		for (size_t i = 0; i < (_uint)OBJID::OBJ_END; i++)
		{
			for (auto obj : objArrList[i])
			{
				result.push_back(obj);
			}
		}
	}

	return result;
}

const vector<CGameObject*> CScene::Get_TotalGameObject(OBJID eObjId)
{
	vector<CGameObject*> result;
	for (auto layer : m_mapLayer)
	{
		auto objArrList = layer.second->Get_AllObject();
		for (auto obj : objArrList[(_uint)eObjId])
		{
			result.push_back(obj);
		}
	}

	return result;
}

HRESULT CScene::Ready_Scene()
{
	return S_OK;
}

_int CScene::Update_Scene(const _float& fTimeDelta)
{
	for (auto& pLayer : m_mapLayer)
		pLayer.second->Update_Layer(fTimeDelta);

	return 0;
}

void CScene::LateUpdate_Scene(const _float& fTimeDelta)
{
	for (auto& pLayer : m_mapLayer)
		pLayer.second->LateUpdate_Layer(fTimeDelta);
}

void CScene::Free()
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();

	Safe_Release(m_pGraphicDev);
}
