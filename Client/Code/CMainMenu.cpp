#include "pch.h"
#include "CMainMenu.h"
#include "CPrototypeManager.h"
#include "CBackGround.h"
#include "CMonster.h"
#include "CTerrain.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontManager.h"

CMainMenu::CMainMenu(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev), m_pLoading(nullptr), m_iFrame(1)
{
}

CMainMenu::~CMainMenu()
{
}

HRESULT CMainMenu::Ready_Scene()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(LAYERID::LAYER_ENVIRONMENT)))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(LAYERID::LAYER_UI)))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);

	if (nullptr == m_pLoading)
		return E_FAIL;

	m_pBackground->Set_IsMainMenu(true);

	return S_OK;
}

_int CMainMenu::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (true == m_pLoading->Get_Finish())					// 임시로 enter누르면 넘어가게
	{
		if (GetAsyncKeyState(VK_RETURN))
		{
			CManagement::GetInstance()->Change_Scene(SCENEID::LOADING);
		}
	}

	m_iFrame += fTimeDelta;
	if (m_iFrame >= 2)
	{
		m_iFrame = 1;
	}

	return iExit;
}

void CMainMenu::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CMainMenu::Render_Scene(const _float& fTimeDelta)
{
	// Debug용 렌더

	_vec2 vPos{ 100.f, 100.f };

	CFontManager::GetInstance()->Render_Font(L"Font_Default", m_pLoading->Get_String(), &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CMainMenu::Ready_Environment_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CBackGround::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_BACKGROUND, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CMainMenu::Ready_UI_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (pLayer == nullptr)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;


	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CMainMenu::Ready_Prototype()
{
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_RectTex", Engine::CRectTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_LogoTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/Title/M_DOOM.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_BackGroundTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/TITLEPIC.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Highlight_Skull", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/M_SKULL%d.png", TEX_NORMAL, 2))))
		return E_FAIL;

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
	Safe_Release(m_pLoading);

	Engine::CScene::Free();
}
