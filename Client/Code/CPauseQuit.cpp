#include "pch.h"
#include "CPauseQuit.h"
#include "CRenderer.h"
#include "CManagement.h"

CPauseQuit::CPauseQuit(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIObject(pGraphicDev)
{
}

CPauseQuit::CPauseQuit(const CPauseQuit& rhs)
	: CUIObject(rhs)
{
}

CPauseQuit::~CPauseQuit()
{
}

HRESULT CPauseQuit::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CPauseQuit::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	CUIObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CPauseQuit::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return;

	if (m_bIsActive)
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

void CPauseQuit::Render_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	// 알파블렌딩 설정
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pBufferCom->Render_Buffer();

	// 렌더 상태 복원
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

_bool CPauseQuit::Action()
{
	PostQuitMessage(0);
	//CManagement::GetInstance()->RequestQuit();

	return false;
}

HRESULT CPauseQuit::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_PauseUI_Quit"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CPauseQuit* CPauseQuit::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPauseQuit* pPauseQuit = new CPauseQuit(pGraphicDev);

	if (FAILED(pPauseQuit->Ready_GameObject()))
	{
		Safe_Release(pPauseQuit);
		MSG_BOX("PauseQuit Create Failed");
		return nullptr;
	}

	return pPauseQuit;
}

CGameObject* CPauseQuit::Clone()
{
	return new CPauseQuit(*this);
}

void CPauseQuit::Free()
{
	Engine::CGameObject::Free();
}
