#include "pch.h"
#include "CGround.h"
#include "CPrototypeManager.h"

CGround::CGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CGround::CGround(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CGround::~CGround()
{
}

HRESULT CGround::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CGround::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CGround::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGround::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CGround::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CTileGround*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_TileGround"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

CGround* CGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGround* pTileGround = new CGround(pGraphicDev);

	if (FAILED(pTileGround->Ready_GameObject()))
	{
		Safe_Release(pTileGround);
		MSG_BOX("TileGround Cretae Failed");
		return nullptr;
	}

	return pTileGround;
}

void CGround::Free()
{
	Engine::CGameObject::Free();
}
