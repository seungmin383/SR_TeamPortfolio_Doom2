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
#include "FaceUIRegistry.h"
#include "CHealth_Small.h"
#include "CHealth_Big.h"
#include "CArmor_Small.h"
#include "CArmor_Big.h"
#include "CAmmo_Clip.h"
#include "CAmmo_Shell.h"
#include "CAmmo_Rocket.h"

#include "CBarrelExplosive.h"
#include "CDoor.h"
#include "CDoorWithKey.h"
#include "CSwitch_Button.h"

#include "CPistol.h"
#include "CWeapon_Shotgun.h"
#include "CWeapon_ChainGun.h"
#include "CWeapon_Rocket.h"
#include "CWeapon_BFG.h"

#include "CStatsManager.h"
#include "CUI_StageClear.h"

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
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYER, OBJID::OBJ_BULLET);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_ENEMY, OBJID::OBJ_BULLET);



	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	CStatsManager::I().Tick(fTimeDelta);

	if (m_isResultMode)
	{
		// 0초로 업데이트: 위치/애니메이션은 그대로, 하지만 렌더 큐엔 올라감
		auto upd0 = [&](LAYERID id) {
			auto it = m_mapLayer.find(id);
			if (it != m_mapLayer.end()) it->second->Update_Layer(0.f);
			};

		upd0(LAYERID::LAYER_CAMERA);       // 뷰/프로젝션 유지
		upd0(LAYERID::LAYER_ENVIRONMENT);  // 스카이박스/지형 등
		upd0(LAYERID::LAYER_GAMELOGIC);    // 적/오브젝트 등(정지된 상태로 그리기)

		// UI는 정상 시간으로
		if (auto it = m_mapLayer.find(LAYERID::LAYER_UI); it != m_mapLayer.end())
			it->second->Update_Layer(fTimeDelta);

		return 0;
	}

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_T) & 0x80)
	{
		CManagement::GetInstance()->Change_Scene(SCENEID::STAGE2);
	}

	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
	CWeaponManager::GetInstance()->Update_Weapon(fTimeDelta);

	CColliderManager::GetInstance()->Update();
}

void CStage::Render_Scene(const _float& fTimeDelta)
{
}

HRESULT CStage::Load_PreSet()
{
	CObjectFactory::GetInstance()->Create_Terrain_Default(VTXCNTX, VTXCNTZ, VTXITV, (_tchar*)L"Proto_TerrainTexture");

	TerrainModels tm = CFileManager::GetInstance()->LoadTerrain((_uint)SCENEID::STAGE1);

	for (auto m : tm.model)
	{
		auto pObj = CObjectFactory::GetInstance()->Load_Terrain(m.vertexCountX,
			m.vertexCountZ,
			m.vertexInterval,
			m.vertices,
			m.strTextureTag,
			m.pos);
	}

	//vector<GameObjectSpawnInfo> loadData = CFileManager::GetInstance()->LoadItem((_uint)SCENEID::STAGE1);;

	//for (auto item : loadData)
	//	CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)item.type1, (OBJID)item.type2, item.position);

	return S_OK;
}

HRESULT CStage::Ready_Environment_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::TERRAIN);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_TERRAIN, pGameObject)))
	//	return E_FAIL;

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
	Engine::CGameObject* pImp = nullptr;
	Engine::CGameObject* pArchvile = nullptr;

	//핑키데몬
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_DEMON);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
		return E_FAIL;

	//아크바일
	pArchvile = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_ARCHVILE);

	if (nullptr == pArchvile)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pArchvile)))
		return E_FAIL;

	//임프
	pImp = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_IMP);

	if (nullptr == pImp)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pImp)))
		return E_FAIL;

	//좀비맨
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_ZOMBIEMAN);

	if (nullptr == pGameObject)
		return E_FAIL;

	auto a = pGameObject;
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
		return E_FAIL;

	//스파이더 마스터마인
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_SPIDERMASTERMIND);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
		return E_FAIL;

	//샷건 가이
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_SHOTGUNGUY);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
		return E_FAIL;

	////임프 불렛
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_IMP_BULLET, pImp);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_BULLET, pGameObject)))
	//	return E_FAIL;

	////아크바일 불렛
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_ARCHVILE_BULLET, pArchvile);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_BULLET, pGameObject)))
	//	return E_FAIL;

	////로켓 불렛
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::PLAYER_ROCKET_BULLET);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_BULLET, pGameObject)))
	//	return E_FAIL;

	// 체력(소)
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_HELTH_SMALL);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 체력(대)
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_HELTH_BIG);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 아머(소)
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_ARMOR_SMALL);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 키
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::KEY);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 아머(대)
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_ARMOR_BIG);

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

	// 로켓 탄약
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_ROCKET);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// BFG 탄약
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ITEM_CELL);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 폭발통
	// 1개만 소환
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::OBJ_BARREL);
	//
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//
	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pGameObject)))						
	//	return E_FAIL;

	/*m_mapLayer.insert({ _eLayerId, pLayer });*/

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
		}

		if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pBarrel)))
			return E_FAIL;
	}


	////�����̴� �����͸��ε�
	//pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_SPIDERMASTERMIND);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))
	//	return E_FAIL;

	// 문 생성(2개)
	Engine::CGameObject* pDoorObj = CPrototypeManager::GetInstance()
		->Clone_ObjPrototype(OBJPROTOTYPE::DOOR);
	if (nullptr == pDoorObj) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pDoorObj))) return E_FAIL;
	dynamic_cast<CDoor*>(pDoorObj)->Set_LinkId(L"A");

	// 문 #1 위치 지정 (확실히)
	Engine::CTransform* tr1 = GetTransformPtr(pDoorObj);
	if (tr1) {
		tr1->Set_Pos(10.f, 2.f, 30.f);   // 기준 위치
	}

	Engine::CGameObject* pDoorObj2 = CPrototypeManager::GetInstance()
		->Clone_ObjPrototype(OBJPROTOTYPE::DOOR);
	if (nullptr == pDoorObj2) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pDoorObj2))) return E_FAIL;
	dynamic_cast<CDoor*>(pDoorObj2)->Set_LinkId(L"A");

	// 문 #2는 왼쪽으로 분리
	Engine::CTransform* tr2 = GetTransformPtr(pDoorObj2);
	if (tr2) tr2->Set_Pos(0.f, 2.f, 30.f);

	// 키로 동작하는 문 제작
	Engine::CGameObject* pDoorObj5 = CPrototypeManager::GetInstance()
		->Clone_ObjPrototype(OBJPROTOTYPE::DOORKEY);
	if (nullptr == pDoorObj5) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pDoorObj5))) return E_FAIL;
	dynamic_cast<CDoorWithKey*>(pDoorObj5)->Set_LinkId(L"B");

	// 문 #3는 오른쪽으로 분리
	Engine::CTransform* tr5 = GetTransformPtr(pDoorObj5);
	if (tr5) tr5->Set_Pos(20.f, 2.f, 30.f);


	// 스위치 생성(1:1)
	Engine::CGameObject* pSwitchObj = CPrototypeManager::GetInstance()
		->Clone_ObjPrototype(OBJPROTOTYPE::BUTTON_DOOR);
	if (nullptr == pSwitchObj) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pSwitchObj))) return E_FAIL;

	auto* sw1 = dynamic_cast<CSwitch_Button*> (pSwitchObj);
	sw1->Set_LinkId(L"A");
	sw1->Set_LinkMode(CSwitch_Button::LinkMode::OneToOne);

	Engine::CTransform* tr3 = GetTransformPtr(pSwitchObj);
	if (tr3) {
		tr3->Set_Pos(15.f, 0.f, 18.f);   // 기준 위치
	}

	// 스위치 생성(1:N)
	Engine::CGameObject* pSwitchObj2 = CPrototypeManager::GetInstance()
		->Clone_ObjPrototype(OBJPROTOTYPE::BUTTON_DOOR);
	if (nullptr == pSwitchObj2) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pSwitchObj2))) return E_FAIL;

	auto* sw2 = dynamic_cast<CSwitch_Button*> (pSwitchObj2);
	sw2->Set_LinkId(L"A");
	sw2->Set_LinkMode(CSwitch_Button::LinkMode::OneToMany);

	Engine::CTransform* tr4 = GetTransformPtr(pSwitchObj2);
	if (tr4) {
		tr4->Set_Pos(5.f, 0.f, 18.f);   // 기준 위치
	}

	// 탈출 스위치
	Engine::CGameObject* pSwitchObj3 = CPrototypeManager::GetInstance()
		->Clone_ObjPrototype(OBJPROTOTYPE::BUTTON_DOOR);
	if (nullptr == pSwitchObj3) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_OBJECT, pSwitchObj3))) return E_FAIL;

	auto* sw3 = dynamic_cast<CSwitch_Button*> (pSwitchObj3);
	sw3->Set_LinkId(L"EXIT");

	Engine::CTransform* tr6 = GetTransformPtr(pSwitchObj3);
	if (tr6) {
		tr6->Set_Pos(25.f, 0.f, 18.f);   // 기준 위치
	}

	// 샷건 오브젝트
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::WEAPON_SHOTGUN);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 체인건 오브젝트
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::WEAPON_CHAINGUN);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// 로켓 런처 오브젝트
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::WEAPON_ROCKET);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	// BFG 오브젝트
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::WEAPON_BFG);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ITEM, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	ConnectDoorSwitchByLinkId();

	// 합계 초기화
	CStatsManager::I().ResetTotals();
	CStatsManager::I().ResetRuntime();

	// 2) 레이어 찾기 (게임 오브젝트가 들어있는 레이어 ID로 교체)
	auto it = m_mapLayer.find(LAYERID::LAYER_GAMELOGIC);
	if (it != m_mapLayer.end()) {
		Engine::CLayer* L = it->second;

		// 3) 몬스터/아이템은 그룹 크기로 바로 집계
		const auto& monsters = L->Get_ObjectGroup(OBJID::OBJ_ENEMY);
		CStatsManager::I().Totals_AddEnemy(static_cast<int>(monsters.size()));

		const auto& items = L->Get_ObjectGroup(OBJID::OBJ_ITEM);
		CStatsManager::I().Totals_AddItem(static_cast<int>(items.size()));

		// 4) 시크릿은 트리거 그룹을 훑어서 조건에 맞는 것만 카운트
		//int secretCnt = 0;
		//for (auto* obj : L->Get_ObjectGroup(OBJID::OBJ_TRIGGER)) {
		//	// 프로젝트 기준에 맞춰 판정(클래스/태그/프로토타입 등)
		//	if (dynamic_cast<CSecretTrigger*>(obj)) ++secretCnt;
		//	// 또는 태그 사용:
		//	// if (obj && obj->HasTag(L"SECRET")) ++secretCnt;
		//}
		//CStatsManager::I().Totals_AddSecret(secretCnt);
	}


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

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))				// 일단 OBJ_WEAPON이라는 enum 값이 있어서 OBJ_UI->OBJ_WEAPON으로 교체(08/05)
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
	CGameUI_Face* pFace = CGameUI_Face::Create(m_pGraphicDev);
	if (!pFace) return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pFace)))
		return E_FAIL;

	// ★ 전역 보관소에 등록
	Client::g_FaceUI = pFace;

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

HRESULT CStage::Ready_Camera_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	CManagement* pManagement = CManagement::GetInstance();

	_vec3	vEye{ 0.f, 0.f, 0.f };
	_vec3	vAt{ 0.f, 0.f, 1.f };
	_vec3	vUp{ 0.f , 1.f, 0.f };

	pGameObject = CDynamicCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_PLAYER, pGameObject)))
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

void CStage::ConnectDoorSwitchByLinkId()
{
	// linkId -> 문 목록
	unordered_map<wstring, vector<CDoor*>> doorsById;
	// linkId -> 스위치 목록
	unordered_map<wstring, vector<CSwitch_Button*>> switchesById;

	const auto& objs = Get_ObjectGroup(LAYERID::LAYER_GAMELOGIC, OBJID::OBJ_OBJECT);

	// 1) 분류
	for (auto* o : objs)
	{
		if (auto* d = dynamic_cast<CDoor*>(o)) {
			const std::wstring& id = d->Get_LinkId();
			if (!id.empty()) doorsById[id].push_back(d);
		}
		else if (auto* s = dynamic_cast<CSwitch_Button*>(o)) {
			const wstring& id = s->Get_LinkId();
			if (!id.empty()) switchesById[id].push_back(s);
		}
	}

	// 2) 연결 (스위치 모드에 따라 1:1 또는 1:N)
	for (const auto& kv : switchesById)
	{
		const wstring& id = kv.first;
		const vector<CSwitch_Button*>& sws = kv.second;

		auto it = doorsById.find(id);
		if (it == doorsById.end()) continue;

		const vector<CDoor*>& candDoors = it->second;

		for (auto* sw : sws)
		{
			// 기존 연결 초기화
			sw->Clear_Targets();
			sw->Set_Target(nullptr);

			if (sw->Get_LinkMode() == CSwitch_Button::LinkMode::OneToMany)
			{
				// 1:N — 같은 ID의 모든 문
				for (auto* d : candDoors)
					sw->Add_Target(d);
			}
			else
			{
				// 1:1 — 같은 ID의 문들 중 가장 가까운 문
				auto* swTr = dynamic_cast<Engine::CTransform*>(
					sw->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform"));

				CDoor* bestDoor = nullptr;
				float bestD2 = (std::numeric_limits<float>::max)();		// 매크로 회피(이렇게 안하면 max 매크로를 인식하지 못함)

				for (auto* d : candDoors)
				{
					auto* dTr = dynamic_cast<Engine::CTransform*>(
						d->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform"));

					if (!swTr || !dTr) { // 트랜스폼 없으면 첫 문으로 대체
						if (!bestDoor) bestDoor = d;
						continue;
					}

					_vec3 a; swTr->Get_Pos(&a);
					_vec3 b; dTr->Get_Pos(&b);

					const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
					const float d2 = dx * dx + dy * dy + dz * dz;
					if (d2 < bestD2) { bestD2 = d2; bestDoor = d; }
				}
				if (bestDoor) sw->Set_Target(bestDoor);
			}
		}
	}
}

void CStage::EnterResultMode()
{
	if (m_isResultMode) return;
	m_isResultMode = true;

	// 결과 UI를 UI 레이어에 추가
	auto it = m_mapLayer.find(LAYERID::LAYER_UI);
	if (it != m_mapLayer.end()) {
		Engine::CLayer* uiLayer = it->second;
		if (auto* ui = CUI_StageClear::Create(m_pGraphicDev)) {
			uiLayer->Add_GameObject(OBJID::OBJ_UI, ui);
		}
	}
}

void CStage::Free()
{
	Engine::CScene::Free();
	CWeaponManager::GetInstance()->DestroyInstance();
}
