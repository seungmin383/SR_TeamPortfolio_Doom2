#include "pch.h"
#include "CTmpWall.h"
#include "CRenderer.h"

CTmpWall::CTmpWall(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CTmpWall::CTmpWall(const CTmpWall& rhs)
	:CGameObject(rhs)
{
}

CTmpWall::~CTmpWall()
{
}

HRESULT CTmpWall::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(100.f, 0.f, 100.f);
	m_pTransformCom->Set_Scale(_vec3{10.f, 10.f, 10.f });

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(10.f, 10.f, 10.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_WALL,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_ENEMY | COLLAYERID::COL_BULLET);

	return S_OK;
}

_int CTmpWall::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	return 0;
}

void CTmpWall::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pColliderCom->Late_Update();
}

void CTmpWall::Render_GameObject(const _float& fTimeDelta)
{
	m_pColliderCom->Render(g_colRend);
}

HRESULT     CTmpWall::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


	return S_OK;
}

CTmpWall* CTmpWall::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTmpWall* pInstance = new CTmpWall(pGraphicDev);
	
	return pInstance;
}

CTmpWall* CTmpWall::Clone()
{
	CTmpWall* pTmpWall = new CTmpWall(*this);

	if (FAILED(pTmpWall->Ready_GameObject()))
	{
		Safe_Release(pTmpWall);
		MSG_BOX("CTmpWall Create Failed");
		return nullptr;
	}

	return pTmpWall;
}

void CTmpWall::Free()
{
	CGameObject::Free();
}