#include "pch.h"
#include "CStage.h"

#include "CTerrain.h"
#include "CSkyBox.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CDynamicCamera.h"
#include "CLightManager.h"
#include "CColliderManager.h"
#include <CPlayerWeapon.h>

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	if (FAILED(Ready_Environment_Layer(LAYERID::LAYER_ENVIRONMENT)))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(LAYERID::LAYER_GAMELOGIC)))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(LAYERID::LAYER_UI)))
		return E_FAIL;
	
	if (FAILED(Ready_Camera_Layer(LAYERID::LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYER, OBJID::OBJ_ENEMTY);

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

	CColliderManager::GetInstance()->Update();
}

void CStage::Render_Scene()
{
}

HRESULT CStage::Ready_Environment_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CTerrain::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_TERRAIN, pGameObject)))
		return E_FAIL;

	pGameObject = CSkyBox::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;
	
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_SKYBOX, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CPlayer::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_PLAYER, pGameObject)))
		return E_FAIL;

	pGameObject = CMonster::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMTY, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CPlayerWeapon::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Camera_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	_vec3	vEye{ 0.f, 10.f, -10.f };
	_vec3	vAt{ 0.f, 0.f, 1.f };
	_vec3	vUp{ 0.f , 1.f, 0.f };

	pGameObject = CDynamicCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_DYNAMICCAMERA, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Light()
{
	D3DLIGHT9 tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tLightInfo.Direction = { 1.f, -1.f, 1.f };

	if (FAILED(CLightManager::GetInstance()->Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	return S_OK;
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage* pStage = new CStage(pGraphicDev);

	if (FAILED(pStage->Ready_Scene()))
	{
		MSG_BOX("Stage Create Failed");
		Safe_Release(pStage);
		return nullptr;
	}

	return pStage;
}

void CStage::Free()
{
	Engine::CScene::Free();
}
