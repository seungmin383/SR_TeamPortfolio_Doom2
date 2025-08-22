#include "pch.h"
#include "CStage2.h"

#include "CSkyBox.h"
#include "CDynamicCamera.h"
#include "CLightManager.h"
#include "CColliderManager.h"
#include "CPlayerWeapon.h"
#include "CManagement.h"
#include "CWeaponManager.h"
#include "CDInputManager.h"

#include "CObjectFactory.h"
#include "CFileManager.h"
#include "../../Engine/Include/TerrainModels.h"
#include "../../Engine/Include/GameObjectSpawnInfo.h"

#include "CGameUI.h"
#include "CGameUI_Info_Weapon.h"
#include "CGameUI_Face.h"
#include "CGameUI_Info.h"
#include "CGameUI_Info_Ammo.h"
#include "CGameUI_Info_WeaponNum.h"
#include "CHealth_Small.h"
#include "CArmor_Small.h"
#include "CArmor_Big.h"
#include "CAmmo_Clip.h"
#include "CAmmo_Shell.h"

#include "CPistol.h"

CStage2::CStage2(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStage2::~CStage2()
{
}

HRESULT CStage2::Ready_Scene()
{
	if (FAILED(Ready_Environment_Layer(LAYERID::LAYER_ENVIRONMENT)))
		return E_FAIL;

	if (FAILED(Ready_Camera_Layer(LAYERID::LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(LAYERID::LAYER_GAMELOGIC)))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(LAYERID::LAYER_UI)))
		return E_FAIL;


	if (FAILED(Ready_Light()))
		return E_FAIL;

	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYER, OBJID::OBJ_ENEMY);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYER, OBJID::OBJ_ITEM);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYER, OBJID::OBJ_OBJECT);

	return S_OK;
}

_int CStage2::Update_Scene(const _float& fTimeDelta)
{
	CColliderManager::GetInstance()->Update();
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_T) & 0x80)
	{
		CManagement::GetInstance()->Change_Scene(SCENEID::STAGE3);
	}

	return iExit;
}

void CStage2::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

}

void CStage2::Render_Scene(const _float& fTimeDelta)
{
}

HRESULT CStage2::Load_PreSet()
{
	CObjectFactory::GetInstance()->Create_Terrain_Default(VTXCNTX, VTXCNTZ, VTXITV, (_tchar*)L"Proto_TerrainTexture");

	TerrainModels tm = CFileManager::GetInstance()->LoadTerrain((_uint)SCENEID::STAGE2);

	for (auto m : tm.model)
	{
		auto pObj = CObjectFactory::GetInstance()->Load_Terrain(m.vertexCountX,
			m.vertexCountZ,
			m.vertexInterval,
			m.vertices,
			m.strTextureTag,
			m.pos);
	}

	//vector<GameObjectSpawnInfo> loadData = CFileManager::GetInstance()->LoadItem((_uint)SCENEID::STAGE2);;

	//for (auto item : loadData)
	//	CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)item.type1, (OBJID)item.type2, item.position);

	return S_OK;
}

HRESULT CStage2::Ready_Environment_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CSkyBox::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_SKYBOX, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CStage2::Ready_GameLogic_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CStage2::Ready_UI_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	// Weapon
	pGameObject = CPlayerWeapon::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_WEAPON, pGameObject)))				// 일단 OBJ_WEAPON이라는 enum 값이 있어서 OBJ_UI->OBJ_WEAPON으로 교체(08/05)
		return E_FAIL;

	// UI
	pGameObject = CGameUI::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))
		return E_FAIL;

	pGameObject = CGameUI_Info_Weapon::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))
		return E_FAIL;

	// Face
	pGameObject = CGameUI_Face::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))
		return E_FAIL;

	// Info
	// (1) UI Info 객체 생성
	pGameObject = CGameUI_Info::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	// (2) Camera 객체 포인터를 얻어서
	const auto& cameraObjects = Get_ObjectGroup(LAYERID::LAYER_CAMERA, OBJID::OBJ_PLAYER);
	if (cameraObjects.empty())
		return E_FAIL;

	auto* pCameraObject = dynamic_cast<CDynamicCamera*>(cameraObjects.back());
	if (pCameraObject == nullptr)
		return E_FAIL;

	// (3) UI Info에 Camera 포인터를 연결
	static_cast<CGameUI_Info*>(pGameObject)->Set_Camera(pCameraObject);

	// (4) UI Info를 UI 레이어에 등록
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))
		return E_FAIL;

	// Ammo
	// (1) UI Info 객체 생성
	pGameObject = CGameUI_Info_Ammo::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	// (2) WeaponManager 포인터 연결
	static_cast<CGameUI_Info_Ammo*>(pGameObject)->Set_WeaponManager(CWeaponManager::GetInstance());

	// (3) UI Info를 UI 레이어에 등록
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))
		return E_FAIL;

	// WeaponNumber
		// (1) UI Info 객체 생성
	pGameObject = CGameUI_Info_WeaponNum::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	// (2) Camera 객체 포인터를 얻어서
	const auto& cameraObjects3 = Get_ObjectGroup(LAYERID::LAYER_CAMERA, OBJID::OBJ_PLAYER);
	if (cameraObjects3.empty())
		return E_FAIL;

	auto* pCameraObject3 = dynamic_cast<CDynamicCamera*>(cameraObjects3.back());
	if (pCameraObject3 == nullptr)
		return E_FAIL;

	// (3) UI Info에 Camera 포인터를 연결
	static_cast<CGameUI_Info_WeaponNum*>(pGameObject)->Set_Camera(pCameraObject3);

	// (4) UI Info를 UI 레이어에 등록
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))
		return E_FAIL;


	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CStage2::Ready_Camera_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	_vec3	vEye{ 0.f, 0.f, 0.f };
	_vec3	vAt{ 0.f, 2.f, 1.f };
	_vec3	vUp{ 0.f , 1.f, 0.f };

	pGameObject = CManagement::GetInstance()->Get_Player();

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_PLAYER, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CStage2::Ready_Light()
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

CStage2* CStage2::Standby(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CStage2(pGraphicDev);
}

CStage2* CStage2::Create()
{
	CStage2* pStage2 = new CStage2(m_pGraphicDev);

	if (FAILED(pStage2->Ready_Scene()))
	{
		MSG_BOX("Stage Create Failed");
		Safe_Release(pStage2);
		return nullptr;
	}

	return pStage2;
}

void CStage2::ConnectDoorSwitchByLinkId()
{
}

void CStage2::Free()
{
	Engine::CScene::Free();
}
