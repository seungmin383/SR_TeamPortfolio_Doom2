#include "pch.h"
#include "CMainApp.h"
#include "CLogo.h"
#include "CStage.h"
#include "CTimerManager.h"
#include "CFrameManager.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CFontManager.h"
#include "CDInputManager.h"
#include "CLightManager.h"
#include "CColliderManager.h"

#include "CTerrain.h"

#include "CDynamicCamera.h"
#include "CZombieMan.h"
#include"CArchVile.h"
#include "CImp.h"
#include "CPinky.h"
#include"CSpiderMastermind.h"
#include "CShotgunGuy.h"

#include "CPlayerWeapon.h"
#include "CHealth_Small.h"
#include "CArmor_Small.h"
#include "CAmmo_Clip.h"
#include "CAmmo_Shell.h"

#include "CBarrelExplosive.h"

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
	// _ulong	dwFlag(0);
	// 
	// if (dwFlag = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z))
	// {
	// 	int a = 0;
	// }

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

	if (FAILED(CDInputManager::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::TERRAIN, CTerrain::Create(m_pGraphicDev))))
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
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_HELTH, CHealth_Small::Create(m_pGraphicDev))))
		return E_FAIL;

	// 아머(소)
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_ARMOR, CArmor_Small::Create(m_pGraphicDev))))
		return E_FAIL;

	// 총알
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_CLIP, CAmmo_Clip::Create(m_pGraphicDev))))
		return E_FAIL;

	// 샷건 탄약
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::ITEM_SHELL, CAmmo_Shell::Create(m_pGraphicDev))))
		return E_FAIL;

	// 폭발통
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ObjPrototype(OBJPROTOTYPE::OBJ_BARREL, CBarrelExplosive::Create(m_pGraphicDev))))
		return E_FAIL;

	//텍스처 필터
	//(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	(*ppGraphicDev)->SetRenderState(D3DRS_ZENABLE, TRUE);		//Z 버퍼에 값을 기록, Z 값 기준 자동 정렬 수행 여부를 물음
	(*ppGraphicDev)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);	//Z 버퍼에 값을 기록할 지 결정

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pManagementClass->Add_Scene(SCENEID::LOGO, new CLogo(pGraphicDev));
	m_pManagementClass->Add_Scene(SCENEID::STAGE1, new CStage(pGraphicDev));
	
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
