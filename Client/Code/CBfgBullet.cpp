#include "pch.h"
#include "Engine_Enum.h"
#include "CBfgBullet.h"
#include "CManagement.h"
#include "CDynamicCamera.h"
#include "CRenderer.h"

CBfgBullet::CBfgBullet(LPDIRECT3DDEVICE9 pGraphicDev) :
	CBullet(pGraphicDev),
	m_eState(BULLET_STATE::IDLE),
	m_iAnimState(0)
{
}

CBfgBullet::CBfgBullet(const CBfgBullet& rhs) :
	CBullet(rhs),
	m_eState(rhs.m_eState),
	m_iAnimState(rhs.m_iAnimState)
{
}

CBfgBullet::~CBfgBullet()
{
	Free();
}

HRESULT CBfgBullet::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	// Set_GameLogicObject 할 때 포지션을 넘겨준다.
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	// Collider Settings
	_vec3 vColliderScale = { 1.f, 1.f, 1.f };
	_vec3 vOffset = { 0.f, 0.f ,0.f };

	m_pColliderCom->Ready_CCollider(this, vPos, vColliderScale, vOffset, COLLAYERID::COL_BULLET,
		COLLAYERID::COL_ENEMY | COLLAYERID::COL_WALL | COLLAYERID::COL_TERRAIN);

	// Animation Setting
	Ready_Animation();

	if (m_pOwner)
	{
		CDynamicCamera* pPlayer = dynamic_cast<CDynamicCamera*>(CManagement::GetInstance()->Get_Player());
		m_vInitDir = pPlayer->Get_At() - pPlayer->Get_Pos();
		D3DXVec3Normalize(&m_vInitDir, &m_vInitDir);
		m_fSpeed = 25.f;
		m_bDead = false;
		m_iDmg = 800;

		_vec3 vScale = { 0.8f, 0.8f, 0.8f };
		m_pTransformCom->Set_Scale(vScale);
	}

	return S_OK;

}

_int CBfgBullet::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead && m_pAnimCom->Get_isFinAct())
		return -1;

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	CBullet::Update_GameObject(fTimeDelta);

	Linear_Movement(fTimeDelta);
	Bill_Board();

    return 0;
}

void CBfgBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);
	m_pColliderCom->Late_Update();

	if (m_eState == BULLET_STATE::HIT)
		m_iAnimState = 1;

	m_pAnimCom->Play_Animation((_uint)m_eState, 0, fTimeDelta);

}

void CBfgBullet::Render_GameObject(const _float& fTimeDelta)
{
	CBullet::Render_GameObject(fTimeDelta);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iAnimState);
	m_pBufferCom->Render_Buffer();
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CBfgBullet::OnCollision(CCollider* _pOther)
{
	if (_pOther->Get_ColLayer() == COLLAYERID::COL_ENEMY)
		_pOther->Get_Owner()->Take_Dmg(m_iDmg);
}

void CBfgBullet::OnCollisionEnter(CCollider* _pOther)
{
	switch (_pOther->Get_ColLayer())
	{
	case COLLAYERID::COL_PLAYER:
		return;

	case COLLAYERID::COL_TERRAIN:
	case COLLAYERID::COL_WALL:
		m_fSpeed = 0.f;
		m_eState = BULLET_STATE::HIT;
		break;

	case COLLAYERID::COL_ENEMY:
		m_fSpeed = 0.f;
		m_eState = BULLET_STATE::HIT;
		_pOther->Get_Owner()->Take_Dmg(m_iDmg);
		break;

	default:
		break;
	}

	// 충돌 후에는 텍스쳐와 콜라이더 스케일을 크게 만든다.
	_vec3 vAfterScale = { 2.f, 2.f, 2.f };
	_vec3 vAfterScale1 = { 6.f, 6.f, 6.f };
	m_pColliderCom->Set_Scale(vAfterScale1);
	m_pTransformCom->Set_Scale(vAfterScale);

}

void CBfgBullet::OnCollisionExit(CCollider* _pOther)
{
	if (_pOther->Get_ColLayer() == COLLAYERID::COL_PLAYER)
		return;

	m_bDead = true;
}

HRESULT CBfgBullet::Add_Component()
{
	// 부모에서 Buffer, Transform, Collider, Animation 컴포넌트 추가 후 ID_DYNAMIC에 삽입
	CBullet::Add_Component();

	// RocketBullet 만의 텍스쳐 추가
	CComponent* pComponent = nullptr;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_BfgBullet"));

	if (pComponent == nullptr)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CBfgBullet::Ready_Animation()
{
	// 날아가는 상태의 애니메이션
	m_pAnimCom->Add_Animation((_uint)BULLET_STATE::IDLE, m_pTextureCom, m_pBufferCom, 2, 1, 0, 1, 0.2f, true, false);

	// 폭발 시의 애니메이션
	m_pAnimCom->Add_Animation((_uint)BULLET_STATE::HIT, m_pTextureCom, m_pBufferCom, 6, 1, 0, 5, 0.15f, false, false);
}

CBfgBullet* CBfgBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBfgBullet* pBfgBullet = new CBfgBullet(pGraphicDev);

	return pBfgBullet;
}


CBfgBullet* CBfgBullet::Clone()
{
	CBfgBullet* pBullet = new CBfgBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return nullptr;
	}

	return pBullet;

}

CBfgBullet* CBfgBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CBfgBullet* pBullet = new CBfgBullet(*this);

	if (pBullet == nullptr)
		return nullptr;

	pBullet->m_pOwner = _Owner;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return nullptr;
	}

	return pBullet;

}
