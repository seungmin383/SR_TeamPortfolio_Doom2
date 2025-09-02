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
#include "CUIManager.h"
#include "CGameObject.h"

#include "CObjectFactory.h"
#include "CFileManager.h"
#include "../../Engine/Include/TerrainModels.h"
#include "../../Engine/Include/GameObjectSpawnInfo.h"

#include "CPausePause.h"
#include "CPauseNew.h"
#include "CPauseQuit.h"
#include "CMainMenuHover.h"

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
#include "CCrosshair.h"

#include "CStatsManager.h"
#include "CUI_StageClear.h"
#include "CVerticalMovingObject.h"
#include "EffectManager.h"
#include "CImpactBulletEffect.h"

#include "CMonsterSpawner.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	ShowCursor(FALSE);

	CUIManager::GetInstance()->UISetListClear();

	CColliderManager::GetInstance()->Reset();

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
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYER, OBJID::OBJ_BUTTON);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYER, OBJID::OBJ_OBJECT);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_BULLET, OBJID::OBJ_OBJECT);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_PLAYER, OBJID::OBJ_WALL);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_ENEMY, OBJID::OBJ_BULLET);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_ENEMY, OBJID::OBJ_ENEMY);
	CColliderManager::GetInstance()->CheckGroup(OBJID::OBJ_ENEMY, OBJID::OBJ_WALL);

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	// 경과 시간
	CStatsManager::I().Tick(fTimeDelta);

	if (m_isResultMode)
	{
		// 0초로 업데이트: 위치/애니메이션은 그대로, 하지만 렌더 큐엔 올라감
		auto upd0 = [&](LAYERID id)
			{
				auto it = m_mapLayer.find(id);
				if (it != m_mapLayer.end()) it->second->Update_Layer(0.f);
			};

		upd0(LAYERID::LAYER_CAMERA);       // 뷰/프로젝션 유지
		upd0(LAYERID::LAYER_ENVIRONMENT);  // 스카이박스/지형 등
		upd0(LAYERID::LAYER_GAMELOGIC);    // 적/오브젝트 등(정지된 상태로 그리기)

		// UI는 정상 시간으로
		if (auto it = m_mapLayer.find(LAYERID::LAYER_UI); it != m_mapLayer.end())
			it->second->Update_Layer(fTimeDelta);

		// 레이어 업데이트가 모두 끝난 뒤에만 씬 전환
		if (m_pendingNext != SCENEID::SCENE_END)
		{
			//if (auto* wm = CWeaponManager::GetInstance()) {
			//	wm->ResetOnSceneChange();   // 무기 포인터 초기화(초기화 하면 메모리 누수 남)
			//}
			CUIManager::GetInstance()->UISetListClear();
			CManagement::GetInstance()->Change_Scene(m_pendingNext);
			m_pendingNext = SCENEID::SCENE_END;
		}

		return 0;
	}

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_T) & 0x80)
	{
		CManagement::GetInstance()->Change_Scene(SCENEID::STAGE2);
	}

	auto uiMgr = CUIManager::GetInstance();
	uiMgr->UpdateUI();
	auto curUiFocus = uiMgr->GetCurUISet()->FocusObserving();
	switch (curUiFocus)
	{
	case 1:
		uiMgr->GetCurUISet()->SetPosition(3, WINCX * 0.5f - 120.f, WINCY * 0.5f - 50.f);
		break;
	case 2:
		uiMgr->GetCurUISet()->SetPosition(3, WINCX * 0.5f - 120.f, WINCY * 0.5f + 50.f);
		break;
	default:
		break;
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
	//지형 설치
	TerrainModels tm = CFileManager::GetInstance()->LoadTerrain((_uint)SCENEID::STAGE1);
	OBJID objid;
	for (auto m : tm.model)
	{
		switch (m.type)
		{
		case 0:
			objid = OBJID::OBJ_TERRAIN;
			break;
		case 1:
			objid = OBJID::OBJ_WALL;
			break;
		case 2:
			objid = OBJID::OBJ_CEIL;
			break;
		default:
			objid = OBJID::OBJ_TERRAIN;
			break;
		}

		auto pObj = CObjectFactory::GetInstance()->Load_Terrain(m.idx,
			m.vertexCountX,
			m.vertexCountZ,
			m.vertexIntervalX,
			m.vertexIntervalZ,
			m.vertexIntervalY,
			m.vertices,
			m.strTextureTag,
			m.pos,
			objid,
			m.blend);
	}

	//오브젝트 설치
	vector<GameObjectSpawnInfo> loadData = CFileManager::GetInstance()->LoadItem((_uint)SCENEID::STAGE1);

	for (auto item : loadData)
	{
		if (item.type3 != (_uint)LINKMODE::NONE && item.tag != "")
			CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)item.type1, (OBJID)item.type2, (LINKMODE)item.type3, item.tag, item.position);
		else
			CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)item.type1, (OBJID)item.type2, item.position);
	}

	ConnectDoorSwitchByLinkId();

	//몬스터 설치
	vector<GameObjectSpawnInfo> loadMonsterData = CFileManager::GetInstance()->LoadMonster((_uint)SCENEID::STAGE1);

	for (auto monster : loadMonsterData)
		CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)monster.type1, (OBJID)monster.type2, monster.position);

	//몬스터 설치 1-1
	////Config.json 참고
	//loadMonsterData = CFileManager::GetInstance()->LoadAdditionalMonster((_uint)SCENEID::STAGE1, 0);

	//for (auto monster : loadMonsterData)
	//	CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)monster.type1, (OBJID)monster.type2, monster.position);

	//몬스터 설치 1-2
	//loadMonsterData = CFileManager::GetInstance()->LoadAdditionalMonster((_uint)SCENEID::STAGE1, 1);

	//for (auto monster : loadMonsterData)
	//	CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)monster.type1, (OBJID)monster.type2, monster.position);

	return S_OK;
}

HRESULT CStage::Ready_Environment_Layer(LAYERID _eLayerId)
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

HRESULT CStage::Ready_GameLogic_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	//좀비맨     
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_ZOMBIEMAN_SPAWNER);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))				// 일단 OBJ_WEAPON이라는 enum 값이 있어서 OBJ_UI->OBJ_WEAPON으로 교체(08/05)
		return E_FAIL;

	//핑키 
	pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::ENEMY_DEMON_SPAWNER);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_ENEMY, pGameObject)))				// 일단 OBJ_WEAPON이라는 enum 값이 있어서 OBJ_UI->OBJ_WEAPON으로 교체(08/05)
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

	// Weapon
	pGameObject = CPlayerWeapon::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pGameObject)))				// 일단 OBJ_WEAPON이라는 enum 값이 있어서 OBJ_UI->OBJ_WEAPON으로 교체(08/05)
		return E_FAIL;

	// UI
	pGameObject = CGameUI::Create(m_pGraphicDev);

	// CrossHair
	CUISet* pUiSet = new CUISet();

	auto crosshair = CCrosshair::Create(m_pGraphicDev);

	if (nullptr == crosshair)
		return E_FAIL;

	crosshair->Set_SizeXY(32.f, 32.f);
	crosshair->Set_Pos(WINCX * 0.5f, WINCY * 0.5f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, crosshair)))
		return E_FAIL;

	pUiSet->InsertUIObject(crosshair, false);


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




	//CUIManager
	//UI 그룹 여러개를 관리
	auto uiMgr = CUIManager::GetInstance();

	//상단의 크로스헤어 추가
	uiMgr->InsertUISet(pUiSet, true, false);

	pUiSet = new CUISet();

	auto pause = CPausePause::Create(m_pGraphicDev);

	if (nullptr == pause)
		return E_FAIL;

	pause->Set_SizeXY(160.f, 30.f);
	pause->Set_Pos(WINCX * 0.5f - 10.f, WINCY * 0.5f - 120.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, pause)))
		return E_FAIL;

	pUiSet->InsertUIObject(pause, false);

	auto newgame = CPauseNew::Create(m_pGraphicDev);

	if (nullptr == newgame)
		return E_FAIL;

	newgame->Set_SizeXY(160.f, 30.f);
	newgame->Set_Pos(WINCX * 0.5f - 10.f, WINCY * 0.5f - 50.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, newgame)))
		return E_FAIL;

	pUiSet->InsertUIObject(newgame, true);

	auto quit = CPauseQuit::Create(m_pGraphicDev);

	if (nullptr == exit)
		return E_FAIL;

	quit->Set_SizeXY(160.f, 30.f);
	quit->Set_Pos(WINCX * 0.5f - 10.f, WINCY * 0.5f + 50.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, quit)))
		return E_FAIL;

	pUiSet->InsertUIObject(quit, true);

	auto hover = CMainMenuHover::Create(m_pGraphicDev);

	if (nullptr == hover)
		return E_FAIL;

	hover->Set_SizeXY(40.f, 40.f);
	hover->Set_Pos(WINCX * 0.5f - 120.f, WINCY * 0.5f + 50.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, hover)))
		return E_FAIL;

	//하나의 UI Set에 그룹에 해당하는 UI 오브젝트들을 추가한다
	pUiSet->InsertUIObject(hover, false);

	//해당 그룹이 받는 key input과 input에 해당하는 메서드 할당
	pUiSet->MappingKeyInput(DIK_UP, bind(&CUISet::AddUIElementIndex, pUiSet));
	pUiSet->MappingKeyInput(DIK_DOWN, bind(&CUISet::SubTractUIElementIndex, pUiSet));
	pUiSet->MappingKeyInput(DIK_RETURN, bind(&CUISet::ActivateUI, pUiSet));
	pUiSet->SetUIElementFocus(1);

	//UI그룹 하나를 매니징에 추가
	uiMgr->InsertUISet(pUiSet, false, true);

	uiMgr->MappingKeyInput(DIK_P, bind(&CUIManager::ToggleUIPhase, uiMgr));
	uiMgr->SetUIPhase(1);




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

	_vec3	vEye{ 65.f, 0.f, 5.f };
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
		if (auto* d = dynamic_cast<CDoor*>(o))
		{
			const std::wstring& id = d->Get_LinkId();
			if (!id.empty()) doorsById[id].push_back(d);
		}
		else if (auto* s = dynamic_cast<CSwitch_Button*>(o))
		{
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

			if (sw->Get_LinkMode() == Engine::LINKMODE::ONE_TO_MANY)
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

					if (!swTr || !dTr)
					{ // 트랜스폼 없으면 첫 문으로 대체
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
	if (it != m_mapLayer.end())
	{
		Engine::CLayer* uiLayer = it->second;
		if (auto* ui = CUI_StageClear::Create(m_pGraphicDev))
		{
			uiLayer->Add_GameObject(OBJID::OBJ_UI, ui);
		}
	}
}

void CStage::RequestNextScene(SCENEID id)
{
	m_pendingNext = id; // 호출만 하고, 실제 전환은 Update_Scene의 끝에서!
}

void CStage::Free()
{
	//CUIManager::GetInstance()->UISetListClear();
	Engine::CScene::Free();
}
