#include "pch.h"
#include "CStage.h"

#include "CTerrain.h"
#include "CSkyBox.h"
#include "CZombieMan.h"
#include "CDynamicCamera.h"
#include "CLightManager.h"
#include "CColliderManager.h"
#include "CPlayerWeapon.h"
#include "CManagement.h"

#include "CGameUI.h"
#include "CGameUI_Face.h"
#include "CGameUI_Info.h"
#include "CGameUI_Info_Ammo.h"
#include "CHealth_Small.h"
#include "CArmor_Small.h"
#include "CAmmo_Clip.h"
#include "CAmmo_Shell.h"

#include "CBarrelExplosive.h"

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
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYERCAMERA, OBJID::OBJ_ENEMY);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYERCAMERA, OBJID::OBJ_ITEM);

	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYERCAMERA, OBJID::OBJ_OBJECT);

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	CColliderManager::GetInstance()->Update();
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

}

void CStage::Render_Scene(const _float& fTimeDelta)
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

	////아크바일
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_ARCHVILE);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
	//	return E_FAIL;

	//m_mapLayer.insert({ _eLayerId, pLayer });

	////임프
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_IMP);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
	//	return E_FAIL;

	//m_mapLayer.insert({ _eLayerId, pLayer });

	////핑키
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_DEMON);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
	//	return E_FAIL;

	//m_mapLayer.insert({ _eLayerId, pLayer });

	//좀비맨
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_ZOMBIEMAN);

	if (nullptr == pGameObject)
		return E_FAIL;

	auto a = pGameObject;
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
		return E_FAIL;

	// 체력(소)
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_HELTH);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 아머(소)
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_ARMOR);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 총알
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_CLIP);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 샷건 탄약
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_SHELL);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 폭발통
	// 1개만 소환
	/*pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::OBJ_BARREL);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pGameObject)))						
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });*/

	// 여러개 소환(테스트)
	std::vector<_vec3> kBarrelPos = {
		_vec3(25.f, 0.f, 25.f),
		_vec3(28.f, 0.f, 25.f),
		_vec3(31.f, 0.f, 25.f),
		_vec3(25.f, 0.f, 28.f),
		_vec3(28.f, 0.f, 28.f),
		_vec3(31.f, 0.f, 28.f),
	};

	for (const auto& p : kBarrelPos)
	{
		auto* pBarrel = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::OBJ_BARREL);
		if (!pBarrel) return E_FAIL;

		// Transform 컴포넌트로 위치/스케일 세팅
		if (auto* tr = dynamic_cast<Engine::CTransform*>(pBarrel->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform")))
		{
			tr->Set_Pos(p.x, p.y, p.z);
			tr->Set_Scale(_vec3(0.2f, 0.2f, 0.2f)); // 필요 시
		}

		if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pBarrel)))
			return E_FAIL;
	}

	////스파이더 마스터마인드
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_SPIDERMASTERMIND);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
	//	return E_FAIL;

	//m_mapLayer.insert({ _eLayerId, pLayer });

	////샷건 가이
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_SHOTGUNGUY);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
	//	return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });
	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(LAYERID _eLayerId)
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
	const auto& cameraObjects = Get_ObjectGroup(LAYERID::LAYER_CAMERA, OBJID::OBJ_PLAYERCAMERA);
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

	// (2) Camera 객체 포인터를 얻어서
	const auto& cameraObjects2 = Get_ObjectGroup(LAYERID::LAYER_CAMERA, OBJID::OBJ_PLAYERCAMERA);
	if (cameraObjects.empty())
		return E_FAIL;

	auto* pCameraObject2 = dynamic_cast<CDynamicCamera*>(cameraObjects2.back());
	if (pCameraObject2 == nullptr)
		return E_FAIL;

	// (3) UI Info에 Camera 포인터를 연결
	static_cast<CGameUI_Info_Ammo*>(pGameObject)->Set_Camera(pCameraObject2);

	// (4) UI Info를 UI 레이어에 등록
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

	CManagement* pManagement = CManagement::GetInstance();

	_vec3	vEye{ 0.f, 0.f, 0.f };
	_vec3	vAt{ 0.f, 2.f, 1.f };
	_vec3	vUp{ 0.f , 1.f, 0.f };

	pGameObject = CDynamicCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_PLAYERCAMERA, pGameObject)))
		return E_FAIL;

	//CManagement에 Player 세팅
	CManagement::GetInstance()->Set_Player(pGameObject);

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

CStage* CStage::Standby(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CStage(pGraphicDev);
}

CStage* CStage::Create()
{
	CStage* pStage = new CStage(m_pGraphicDev);

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
