#include "pch.h"
#include "CMainApp.h"
#include "CLogo.h"
#include "CTimerManager.h"
#include "CFrameManager.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"

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
	m_pManagementClass->Update_Scene(fTimeDelta);

	return 0;
}

void CMainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
	m_pManagementClass->LateUpdate_Scene(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	m_pManagementClass->Render_Scene(m_pGraphicDev);

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

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Engine::CScene* pScene = CLogo::Create(pGraphicDev);
	if (nullptr == pScene)
		return E_FAIL;

	if (FAILED(m_pManagementClass->Set_Scene(pScene)))
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
	
	CRenderer::GetInstance()->DestroyInstance();
	CPrototypeManager::GetInstance()->DestroyInstance();
	CTimerManager::GetInstance()->DestroyInstance();
	CFrameManager::GetInstance()->DestroyInstance();

	m_pManagementClass->DestroyInstance();
	m_pDeviceClass->DestroyInstance();
}
