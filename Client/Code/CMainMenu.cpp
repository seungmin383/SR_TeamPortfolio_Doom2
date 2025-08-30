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

	// 키보드 포커스 업데이트
	HandleKeyboard(fTimeDelta);

	// 마우스가 버튼 위에 있으면 포커스를 동기화(자연스러운 UX)
	if (m_hover != -1) m_focus = m_hover;

	// m_hover == 0 이면 메뉴 왼쪽에 해골 이미지 출력

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
	// 간단한 디바운스
	m_inputLock = max(0.f, m_inputLock - fTimeDelta);
	auto pressOnce = [&](int vk)->bool {
		if (GetAsyncKeyState(vk) & 0x8000) {
			if (m_inputLock <= 0.f) { m_inputLock = 0.15f; return true; }
		}
		return false;
		};

	// ↑ / ↓ 로 포커스 이동
	if (pressOnce(VK_UP)) { m_focus = (m_focus + 1 + 1) % 2; PlaySfx_Move(); }   // 0/1 전환
	if (pressOnce(VK_DOWN)) { m_focus = (m_focus + 1) % 2;     PlaySfx_Move(); }

	// Enter/Space 로 확정
	if (pressOnce(VK_RETURN) || pressOnce(VK_SPACE)) {
		ActivateFocused();
	}
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

	Engine::CGameObject* pGameObject = nullptr;

	auto background = CMainMenuBackground::Create(m_pGraphicDev);

	if (nullptr == background)
		return E_FAIL;

	background->Set_SizeXY(WINCX, WINCY);
	background->Set_Pos(WINCX * 0.5f, WINCY * 0.5f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, background)))
		return E_FAIL;

	auto logo = CMainMenuLogo::Create(m_pGraphicDev);
	
	if (nullptr == logo)
		return E_FAIL;

	logo->Set_SizeXY(100.f, 10.f);
	logo->Set_Pos(WINCX * 0.5f - 10.f, WINCY * 0.5f - 50.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, logo)))
		return E_FAIL;

	auto exit = CMainMenuExit::Create(m_pGraphicDev);

	if (nullptr == exit)
		return E_FAIL;

	exit->Set_SizeXY(100.f, 10.f);
	exit->Set_Pos(WINCX * 0.5f - 10.f, WINCY * 0.5f + 50.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, exit)))
		return E_FAIL;

	auto hover = CMainMenuHover::Create(m_pGraphicDev);

	if (nullptr == hover)
		return E_FAIL;
	
	hover->Set_SizeXY(15.f, 15.f);
	hover->Set_Pos(WINCX * 0.5f - 90.f, WINCY * 0.5f + 50.f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, hover)))
		return E_FAIL;

	auto uiCamera = CUICamera::Create(m_pGraphicDev);

	if (nullptr == uiCamera)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, uiCamera)))
		return E_FAIL;

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
