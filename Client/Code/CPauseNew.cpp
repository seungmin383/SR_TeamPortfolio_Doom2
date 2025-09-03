#include "pch.h"
#include "CPauseNew.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CWeaponManager.h"

CPauseNew::CPauseNew(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIObject(pGraphicDev)
{
}

CPauseNew::CPauseNew(const CPauseNew& rhs)
	: CUIObject(rhs)
{
}

CPauseNew::~CPauseNew()
{
}

HRESULT CPauseNew::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CPauseNew::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	CUIObject::Update_GameObject(fTimeDelta);

	if (m_bIsActive)
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CPauseNew::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return;

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPauseNew::Render_GameObject(const _float& fTimeDelta)
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

_bool CPauseNew::Action()
{
	if (CManagement::GetInstance()->Get_CurSceneId() == SCENEID::STAGE1)
		CWeaponManager::GetInstance()->DestroyInstance();
	
	CManagement::GetInstance()->Set_Paused(false);

	CManagement::GetInstance()->Change_Scene(CManagement::GetInstance()->Get_CurSceneId());

	return false;
}

HRESULT CPauseNew::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_PauseUI_New"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CPauseNew* CPauseNew::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPauseNew* pPauseNew = new CPauseNew(pGraphicDev);

	if (FAILED(pPauseNew->Ready_GameObject()))
	{
		Safe_Release(pPauseNew);
		MSG_BOX("PauseNew Create Failed");
		return nullptr;
	}

	return pPauseNew;
}

CGameObject* CPauseNew::Clone()
{
	return new CPauseNew(*this);
}

void CPauseNew::Free()
{
	Engine::CGameObject::Free();
}
