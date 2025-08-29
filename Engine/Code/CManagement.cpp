#include "CManagement.h"
#include "CRenderer.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pScene(nullptr)
{
	for (size_t i = 0; i < (_uint)SCENEID::SCENE_END; i++)
		m_pArrScene[i] = nullptr;
}

CManagement::~CManagement()
{
	Free();
}

CComponent* CManagement::Get_Component(COMPONENTID eID, LAYERID _eLayerId, OBJID _eObjId, const _tchar* pComponentTag)
{
	if (nullptr == m_pScene)
	{
		return nullptr;
	}

	return m_pScene->Get_Component(eID, _eLayerId, _eObjId, pComponentTag);
}

const vector<CGameObject*>& CManagement::Get_ObjectGroup(LAYERID _eLayerId, OBJID _eObjId)
{
	vector<CGameObject*> result;

	if (nullptr == m_pScene)
	{
		return result;
	}

	return m_pScene->Get_ObjectGroup(_eLayerId, _eObjId);
}

HRESULT CManagement::Set_Scene(CScene* pScene)
{
	if (nullptr == pScene)
		return E_FAIL;

	Safe_Release(m_pScene);

	CRenderer::GetInstance()->Clear_RenderGroup();

	m_pScene = pScene;

	m_pScene->Load_PreSet();

	return S_OK;
}

HRESULT CManagement::Add_Scene(SCENEID _eSceneId, CScene* _pScene)
{
	if (nullptr == _pScene)
		return E_FAIL;

	m_pArrScene[(_uint)_eSceneId] = _pScene;

	return S_OK;
}

HRESULT CManagement::Change_Scene(SCENEID _eSceneId)
{
	//m_pArrScene[(_uint)_eSceneId] = m_pArrScene[(_uint)_eSceneId]->Create();

	if (nullptr == m_pArrScene[(_uint)_eSceneId])
		return E_FAIL;

	Set_Scene(m_pArrScene[(_uint)_eSceneId]->Create());

	m_eCurSceneId = _eSceneId;

	return S_OK;
}

HRESULT CManagement::Set_Player(CGameObject* pObj)
{
	if (nullptr == pObj)
		return E_FAIL;

	m_pPlayer = pObj;

	return S_OK;
}

HRESULT CManagement::Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, _int iCnt)
{
	m_pScene->Set_GameLogicObject(eObjPrototype, eObjId, vPos, iCnt);

	return S_OK;
}

HRESULT CManagement::Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, CGameObject* _Owner, CGameObject*& _Bullet)
{
	m_pScene->Set_GameLogicObject(eObjPrototype, eObjId, vPos, _Owner, _Bullet);
	
	return S_OK;
}

HRESULT CManagement::Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId,
	_vec3 vPos, _int iCnt,
	const std::wstring& linkId,
	LINKMODE linkMode)
{
	if (!m_pScene)
		return E_FAIL;

	// Get_Layer()가 맵을 돌려줄 때: 거기서 GAMELOGIC 레이어 포인터를 꺼냅니다.
	const auto& layerMap = m_pScene->Get_Layer();  // std::map<LAYERID, Engine::CLayer*>
	auto it = layerMap.find(LAYERID::LAYER_GAMELOGIC);
	if (it == layerMap.end() || it->second == nullptr) return E_FAIL;

	Engine::CLayer* pLayer = it->second;

	// Layer 오버로드에 그대로 위임 (즉시 주입 버전)
	return pLayer->SetOn_GameLogicObject(eObjPrototype, eObjId, vPos, iCnt, linkId, linkMode);
}

_int CManagement::Update_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pScene)
		return -1;

	return m_pScene->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene(const _float& fTimeDelta)
{
	m_pScene->LateUpdate_Scene(fTimeDelta);
}

void CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Render_GameObejct(pGraphicDev, fTimeDelta);

	m_pScene->Render_Scene(fTimeDelta);
}

void CManagement::Free()
{
	//프로그램 종료라고 판단, 강제로 stage3으로 설정
	//Layer 소멸 시 player를 포함한 모든 오브젝트 release 의도
	//m_eCurSceneId = SCENEID::STAGE3;
	m_eCurSceneId = SCENEID::TEST;

	for (size_t i = 0; i < (_uint)SCENEID::SCENE_END; i++)
	{
		if (nullptr != m_pArrScene[i])
			Safe_Release(m_pArrScene[i]);
	}

	Safe_Release(m_pScene);
}
