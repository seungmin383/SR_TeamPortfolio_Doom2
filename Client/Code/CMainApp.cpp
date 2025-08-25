#include "pch.h"
#include "CMainApp.h"

#include "CLogo.h"
#include "CMainMenu.h"
#include "CStage.h"
#include "CStage2.h"
#include "CStage3.h"

#include "CTimerManager.h"
#include "CFrameManager.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CFontManager.h"
#include "CDInputManager.h"
#include "CLightManager.h"
#include "CColliderManager.h"
#include "CWeaponManager.h"
#include "CFileManager.h"
#include "CObjectFactory.h"
#include "CTextureManager.h"

#include "CTerrain.h"
#include "CTerrainSector.h"

#include "CDynamicCamera.h"
#include "CZombieMan.h"
#include "CArchVile.h"
#include "CImp.h"
#include "CPinky.h"
#include "CSpiderMastermind.h"
#include "CShotgunGuy.h"
#include "CImpBullet.h"
#include "CArchvileBullet.h"

#include "CPlayerWeapon.h"
#include "CHealth_Small.h"
#include "CHealth_Big.h"
#include "CArmor_Small.h"
#include "CArmor_Big.h"
#include "CAmmo_Clip.h"
#include "CAmmo_Shell.h"
#include "CAmmo_Rocket.h"
#include "CAmmo_Cell.h"

#include "CWeapon_Shotgun.h"
#include "CWeapon_ChainGun.h"
#include "CWeapon_Rocket.h"
#include "CWeapon_BFG.h"
#include "CRocketBullet.h"
#include "CBfgBullet.h"

#include "CBarrelExplosive.h"
#include "CDoor.h"
#include "CDoorWithKey.h"
#include "CKey.h"
#include "CSwitch_Button.h"

CMainApp::CMainApp()
	: m_pGraphicDev(nullptr),
	m_pManagementClass(CManagement::GetInstance())
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(Ready_Scene(m_pGraphicDev)))
		return E_FAIL;

	return S_OK;
}

int CMainApp::Update_MainApp(const float& fTimeDelta)
{
	CDInputManager::GetInstance()->Update_InputDev();

	m_pManagementClass->Update_Scene(fTimeDelta);

	if (GetAsyncKeyState('C'))
		g_colRend = !g_colRend;

	return 0;
}

void CMainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
	m_pManagementClass->LateUpdate_Scene(fTimeDelta);
}

void CMainApp::Render_MainApp(const float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	m_pManagementClass->Render_Scene(m_pGraphicDev, fTimeDelta);

	m_pDeviceClass->Render_End();
}

HRESULT CMainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	srand(time(NULL));

	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd,
		MODE_WIN, WINCX, WINCY, &m_pDeviceClass)))
	{
		MSG_BOX("CGraphicDev Ready Failed");
		return E_FAIL;
	}

	m_pDeviceClass->Add_Ref();

	(*ppGraphicDev) = CGraphicDev::GetInstance()->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	if (FAILED(CFontManager::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Default", L"바탕", 20, 15, FW_HEAVY)))
		return E_FAIL;

	if (FAILED(CFontManager::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Jinji", L"궁서", 20, 10, FW_THIN)))
		return E_FAIL;

	if (FAILED(CFontManager::GetInstance()->Ready_Font(m_pGraphicDev, L"ResultTitle", L"Arial", 28, 32, 900)))
		return E_FAIL;

	if (FAILED(CFontManager::GetInstance()->Ready_Font(m_pGraphicDev, L"ResultBody", L"Arial", 18, 24, 700)))
		return E_FAIL;

	if (FAILED(CDInputManager::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::TERRAIN, CTerrain::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::TERRAINSECTOR, CTerrainSector::Create(m_pGraphicDev))))
		return E_FAIL;

	//Initiate GameObject Prototype
	//좀비맨
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ENEMY_ZOMBIEMAN, CZombieMan::Create(m_pGraphicDev))))
		return E_FAIL;
	//아크바일
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ENEMY_ARCHVILE, CArchVile::Create(m_pGraphicDev))))
		return E_FAIL;
	//임프
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ENEMY_IMP, CImp::Create(m_pGraphicDev))))
		return E_FAIL;
	//핑키
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ENEMY_DEMON, CPinky::Create(m_pGraphicDev))))
		return E_FAIL;
	//스파이더 마스터마인드
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ENEMY_SPIDERMASTERMIND, CSpiderMastermind::Create(m_pGraphicDev))))
		return E_FAIL;
	//샷건 가이
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ENEMY_SHOTGUNGUY, CShotgunGuy::Create(m_pGraphicDev))))
		return E_FAIL;

	// 체력(소)
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_HELTH_SMALL, CHealth_Small::Create(m_pGraphicDev))))
		return E_FAIL;

	// 체력(대)
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_HELTH_BIG, CHealth_Big::Create(m_pGraphicDev))))
		return E_FAIL;

	// 아머(소)
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_ARMOR_SMALL, CArmor_Small::Create(m_pGraphicDev))))
		return E_FAIL;

	// 아머(대)
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_ARMOR_BIG, CArmor_Big::Create(m_pGraphicDev))))
		return E_FAIL;

	// 총알
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_CLIP, CAmmo_Clip::Create(m_pGraphicDev))))
		return E_FAIL;

	// 샷건 탄약
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_SHELL, CAmmo_Shell::Create(m_pGraphicDev))))
		return E_FAIL;

	// 로켓 탄약
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_ROCKET, CAmmo_Rocket::Create(m_pGraphicDev))))
		return E_FAIL;

	// BFG 탄약
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_CELL, CAmmo_Cell::Create(m_pGraphicDev))))
		return E_FAIL;

	// 폭발통
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::OBJ_BARREL, CBarrelExplosive::Create(m_pGraphicDev))))
		return E_FAIL;

	// 문
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::DOOR, CDoor::Create(m_pGraphicDev))))
		return E_FAIL;

	// 키로 동작하는 문
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::DOORKEY, CDoorWithKey::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::KEY, CKey::Create(m_pGraphicDev))))
		return E_FAIL;

	// 스위치
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::BUTTON_DOOR, CSwitch_Button::Create(m_pGraphicDev))))
		return E_FAIL;

	// 샷건
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::WEAPON_SHOTGUN, CWeapon_Shotgun::Create(m_pGraphicDev))))
		return E_FAIL;

	// 체인건
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::WEAPON_CHAINGUN, CWeapon_ChainGun::Create(m_pGraphicDev))))
		return E_FAIL;

	// 로켓
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::WEAPON_ROCKET, CWeapon_Rocket::Create(m_pGraphicDev))))
		return E_FAIL;
	
	// BFG
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::WEAPON_BFG, CWeapon_BFG::Create(m_pGraphicDev))))
		return E_FAIL;

	// 임프총알
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ENEMY_IMP_BULLET, CImpBullet::Create(m_pGraphicDev))))
		return E_FAIL;

	// 아크바일 총알
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ENEMY_ARCHVILE_BULLET, CArchvileBullet::Create(m_pGraphicDev))))
		return E_FAIL;

	// 로켓 총알
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::PLAYER_ROCKET_BULLET, CRocketBullet::Create(m_pGraphicDev))))
		return E_FAIL;

	// BFG 총알
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::PLAYER_BFG_BULLET, CBfgBullet::Create(m_pGraphicDev))))
		return E_FAIL;


	//텍스처 필터
	//(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	(*ppGraphicDev)->SetRenderState(D3DRS_ZENABLE, TRUE);		//Z 버퍼에 값을 기록, Z 값 기준 자동 정렬 수행 여부를 물음
	(*ppGraphicDev)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);	//Z 버퍼에 값을 기록할 지 결정

	CObjectFactory::GetInstance()->Set_Dev(*ppGraphicDev);

	CFileManager::GetInstance()->SetProjectDir();

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pManagementClass->Add_Scene(SCENEID::LOGO, new CLogo(pGraphicDev));
	m_pManagementClass->Add_Scene(SCENEID::MAINMENU, new CMainMenu(pGraphicDev));
	m_pManagementClass->Add_Scene(SCENEID::STAGE1, new CStage(pGraphicDev));
	m_pManagementClass->Add_Scene(SCENEID::STAGE2, new CStage2(pGraphicDev));
	m_pManagementClass->Add_Scene(SCENEID::STAGE3, new CStage3(pGraphicDev));

	if (FAILED(m_pManagementClass->Change_Scene(SCENEID::LOGO)))
	{
		MSG_BOX("Scene Setting Failed");
		return E_FAIL;
	}

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Ready_MainApp()))
	{
		delete pMainApp;
		pMainApp = nullptr;
	}

	return pMainApp;
}

void CMainApp::Free()
{
	Engine::Safe_Release(m_pDeviceClass);
	Engine::Safe_Release(m_pGraphicDev);
	
	CTextureManager::GetInstance()->DestroyInstance();
	CFileManager::GetInstance()->DestroyInstance();
	CObjectFactory::GetInstance()->DestroyInstance();
	CColliderManager::GetInstance()->DestroyInstance();
	CLightManager::GetInstance()->DestroyInstance();
	CFontManager::GetInstance()->DestroyInstance();
	CRenderer::GetInstance()->DestroyInstance();
	CPrototypeManager::GetInstance()->DestroyInstance();
	CTimerManager::GetInstance()->DestroyInstance();
	CFrameManager::GetInstance()->DestroyInstance();
	CDInputManager::GetInstance()->DestroyInstance();

	m_pManagementClass->DestroyInstance();
	m_pDeviceClass->DestroyInstance();
}
