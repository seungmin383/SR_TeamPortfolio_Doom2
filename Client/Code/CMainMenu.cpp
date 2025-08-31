#include "pch.h"
#include "CMainMenu.h"
#include "CBackGround.h"
#include "CManagement.h"
#include "CFontManager.h"
#include "CTextureManager.h"
#include "CLayer.h"
#include "Define.h"

#include "CMainMenuBackground.h"
#include "CMainMenuLogo.h"
#include "CMainMenuExit.h"
#include "CMainMenuHover.h"
#include "CUICamera.h"

#include "CUISet.h"
#include "CUIManager.h"

CMainMenu::CMainMenu(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CMainMenu::~CMainMenu()
{
}

HRESULT CMainMenu::Ready_Scene()
{
	if (FAILED(Ready_UI_Layer(LAYERID::LAYER_UI)))
		return E_FAIL;

	m_focus = 0; // START

	return S_OK;
}

_int CMainMenu::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

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

void CMainMenu::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CMainMenu::Render_Scene(const _float& fTimeDelta)
{
}

HRESULT CMainMenu::Load_PreSet()
{
	return S_OK;
}

void CMainMenu::HandleKeyboard(_float fTimeDelta)
{
	//// 간단한 디바운스
	//m_inputLock = max(0.f, m_inputLock - fTimeDelta);
	//auto pressOnce = [&](int vk)->bool {
	//	if (GetAsyncKeyState(vk) & 0x8000) {
	//		if (m_inputLock <= 0.f) { m_inputLock = 0.15f; return true; }
	//	}
	//	return false;
	//	};

	//// ↑ / ↓ 로 포커스 이동
	//if (pressOnce(VK_UP)) { m_focus = (m_focus + 1 + 1) % 2; PlaySfx_Move(); }   // 0/1 전환
	//if (pressOnce(VK_DOWN)) { m_focus = (m_focus + 1) % 2;     PlaySfx_Move(); }

	//// Enter/Space 로 확정
	//if (pressOnce(VK_RETURN) || pressOnce(VK_SPACE)) {
	//	ActivateFocused();
	//}
}

void CMainMenu::ActivateFocused()
{
	if (m_focus == 0)
		CManagement::GetInstance()->Change_Scene(SCENEID::STAGE1);
	else
		CManagement::GetInstance()->RequestQuit();
}

void CMainMenu::PlaySfx_Move()
{
}

void CMainMenu::PlaySfx_Select()
{
}

HRESULT CMainMenu::Ready_UI_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (pLayer == nullptr)
		return E_FAIL;

	//CUISet
	//UI 한 그룹
	CUISet* pUiSet = new CUISet();

	//CUIManager
	//UI 그룹 여러개를 관리
	auto uiMgr = CUIManager::GetInstance();

	auto background = CMainMenuBackground::Create(m_pGraphicDev);

	if (nullptr == background)
		return E_FAIL;

	background->Set_SizeXY(WINCX, WINCY);
	background->Set_Pos(WINCX * 0.5f, WINCY * 0.5f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, background)))
		return E_FAIL;

	pUiSet->InsertUIObject(background, false);

	auto logo = CMainMenuLogo::Create(m_pGraphicDev);
	
	if (nullptr == logo)
		return E_FAIL;

	logo->Set_SizeXY(160.f, 30.f);
	logo->Set_Pos(WINCX * 0.5f - 10.f, WINCY * 0.5f - 50.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, logo)))
		return E_FAIL;

	pUiSet->InsertUIObject(logo, true);

	auto exit = CMainMenuExit::Create(m_pGraphicDev);

	if (nullptr == exit)
		return E_FAIL;

	exit->Set_SizeXY(160.f, 30.f);
	exit->Set_Pos(WINCX * 0.5f - 10.f, WINCY * 0.5f + 50.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, exit)))
		return E_FAIL;

	pUiSet->InsertUIObject(exit, true);

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
	uiMgr->InsertUISet(pUiSet);

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CMainMenu::Ready_Prototype()
{
	return S_OK;
}

CMainMenu* CMainMenu::Standby(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CMainMenu(pGraphicDev);
}

CMainMenu* CMainMenu::Create()
{
	CMainMenu* pMainMenu = new CMainMenu(m_pGraphicDev);

	if (FAILED(pMainMenu->Ready_Scene()))
	{
		MSG_BOX("MainMenu Create Failed");
		Safe_Release(pMainMenu);
		return nullptr;
	}

	return pMainMenu;
}

void CMainMenu::Free()
{
	Engine::CScene::Free();
}
