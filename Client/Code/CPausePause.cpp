#include "pch.h"
#include "CPausePause.h"
#include "CRenderer.h"

CPausePause::CPausePause(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIObject(pGraphicDev)
{
}

CPausePause::CPausePause(const CPausePause& rhs)
	: CUIObject(rhs)
{
}

CPausePause::~CPausePause()
{
}

HRESULT CPausePause::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CPausePause::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	CUIObject::Update_GameObject(fTimeDelta);

	if (m_bIsActive)
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CPausePause::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return;

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPausePause::Render_GameObject(const _float& fTimeDelta)
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

_bool CPausePause::Action()
{
	//DO SOMETHING
	return true;
}

HRESULT CPausePause::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_PauseUI_Pause"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CPausePause* CPausePause::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPausePause* pPausePause = new CPausePause(pGraphicDev);

	if (FAILED(pPausePause->Ready_GameObject()))
	{
		Safe_Release(pPausePause);
		MSG_BOX("PausePause Create Failed");
		return nullptr;
	}

	return pPausePause;
}

CGameObject* CPausePause::Clone()
{
	return new CPausePause(*this);
}

void CPausePause::Free()
{
	Engine::CGameObject::Free();
}
