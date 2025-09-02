#include "pch.h"
#include "CMonsterSpawner.h"
#include "CRenderer.h"

CMonsterSpawner::CMonsterSpawner(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CMonsterSpawner::CMonsterSpawner(const CMonsterSpawner& rhs)
	:CGameObject(rhs), m_ObjType(rhs.m_ObjType)
{
}

CMonsterSpawner::~CMonsterSpawner()
{
}

HRESULT CMonsterSpawner::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3{ 10.f, 10.f, 10.f });

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(10.f, 10.f, 10.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_WALL,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_ENEMY | COLLAYERID::COL_BULLET);

	return S_OK;
}

_int CMonsterSpawner::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	return 0;
}

void CMonsterSpawner::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pColliderCom->Late_Update();
}

void CMonsterSpawner::Render_GameObject(const _float& fTimeDelta)
{
	m_pColliderCom->Render(g_colRend);
}

HRESULT     CMonsterSpawner::Add_Component()
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

CMonsterSpawner* CMonsterSpawner::Create(LPDIRECT3DDEVICE9 pGraphicDev, OBJPROTOTYPE _Type)
{
	CMonsterSpawner* pInstance = new CMonsterSpawner(pGraphicDev);

	pInstance->m_ObjType = _Type;

	return pInstance;
}

CMonsterSpawner* CMonsterSpawner::Clone()
{
	CMonsterSpawner* pTmpWall = new CMonsterSpawner(*this);

	if (FAILED(pTmpWall->Ready_GameObject()))
	{
		Safe_Release(pTmpWall);
		MSG_BOX("CMonsterSpawner Create Failed");
		return nullptr;
	}

	return pTmpWall;
}

void CMonsterSpawner::Free()
{
	CGameObject::Free();
}