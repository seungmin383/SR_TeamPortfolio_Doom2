#include "pch.h"
#include "CBullet.h"
#include "CManagement.h"
#include <CDynamicCamera.h>

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev),
	m_iDamage(0), m_vDir(_vec3{0.f,0.f,0.f}),
	m_BulletType(MONSTER_BULLET_TYPE::BULLET_END)
{
}

CBullet::CBullet(const CBullet& rhs)
	:CGameObject(rhs),
	m_iDamage(rhs.m_iDamage), m_vDir(_vec3{ 0.f,0.f,0.f }),
	m_BulletType(rhs.m_BulletType)
{
}

CBullet::~CBullet()
{

}

HRESULT CBullet::Add_Component()
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectSpriteTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectSpriteTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//MoveTransform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_MoveTransform", pComponent });

	//Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	//Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	//Animation
	pComponent = m_pAnimCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;

}

void CBullet::OnCollision(CCollider* _pOther)
{
}

void CBullet::OnCollisionEnter(CCollider* _pOther)
{
	if (_pOther == CManagement::GetInstance()->Get_Player()->GetCollider())
		dynamic_cast<CDynamicCamera*>(_pOther->Get_Owner())->Take_Damage(m_iDamage);
}

void CBullet::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CBullet::Ready_GameObject()
{
	Add_Component();

	if(Movement_Strategy)
		Movement_Strategy->Ready_Movement(this);

	if(Impact_Strategy)
		Impact_Strategy->Ready_Impact(this);

	return S_OK;
}

_int CBullet::Update_GameObject(const _float& fTimeDelta)
{
	if(Movement_Strategy)
		Movement_Strategy->Update_Movement(this, fTimeDelta);

	if(Impact_Strategy)
		Impact_Strategy->Update_Impact(this, fTimeDelta);

	return 0;
}

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBullet* Bullet = new CBullet(pGraphicDev);

	if (Bullet == nullptr)
		return nullptr;

	if (FAILED(Bullet->Ready_GameObject()))
	{
		Safe_Release(Bullet);
		MSG_BOX("총알 생성 실패");
		return nullptr;
	}

	return Bullet;
}

CGameObject* CBullet::Clone()
{
	CBullet* PBullet = new CBullet(*this);

	if (FAILED(PBullet->Ready_GameObject()))
	{
		Safe_Release(PBullet);
		MSG_BOX("ArchVile Create Failed");
		return nullptr;
	}

	return PBullet;
}

void CBullet::Free()
{
	CGameObject::Free();
}

