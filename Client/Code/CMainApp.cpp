#include "pch.h"
#include "CMainApp.h"
#include "CLogo.h"
#include "CTimerManager.h"
#include "CFrameManager.h"
#include "CPrototypeManager.h"

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

	m_pGraphicDev = CGraphicDev::GetInstance()->Get_GraphicDev();
	m_pGraphicDev->AddRef();

	//юс╫ц
	_matrix		matView, matProj;
	_vec3		vEye{ 0.f, 10.f, -10.f };
	_vec3		vAt{ 0.f, 0.f, 1.f };
	_vec3		vUp{ 0.f, 1.f, 0.f };

	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	(*ppGraphicDev)->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.f), float(WINCX) / WINCY, 0.1f, 1000.f);
	(*ppGraphicDev)->SetTransform(D3DTS_PROJECTION, &matProj);
	//

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
	
	CPrototypeManager::GetInstance()->DestroyInstance();
	CTimerManager::DestroyInstance();
	CFrameManager::DestroyInstance();
	CManagement::DestroyInstance();

	m_pDeviceClass->DestroyInstance();
}
