#include "pch.h"
#include "Engine_Enum.h"
#include "CShotgunBullet.h"
#include "CManagement.h"
#include "CDynamicCamera.h"
#include "CRenderer.h"

CShotgunBullet::CShotgunBullet(LPDIRECT3DDEVICE9 pGraphicDev) :
	CBullet(pGraphicDev)
{
}

CShotgunBullet::CShotgunBullet(const CShotgunBullet& rhs) :
	CBullet(rhs)
{
}

CShotgunBullet::~CShotgunBullet()
{
	Free();
}

HRESULT CShotgunBullet::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	// Set_GameLogicObject 할 때 포지션을 넘겨준다.
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	// Collider Settings
	_vec3 vColliderScale = { 0.1f, 0.1f, 0.1f };
	_vec3 vOffset = { 0.f, 0.f ,0.f };

	m_pColliderCom->Ready_CCollider(this, vPos, vColliderScale, vOffset, COLLAYERID::COL_BULLET,
		COLLAYERID::COL_ENEMY | COLLAYERID::COL_WALL | COLLAYERID::COL_TERRAIN);

	if (m_pOwner)
	{
		CDynamicCamera* pPlayer = dynamic_cast<CDynamicCamera*>(CManagement::GetInstance()->Get_Player());
		m_vInitDir = pPlayer->Get_At() - pPlayer->Get_Pos();
		D3DXVec3Normalize(&m_vInitDir, &m_vInitDir);
		m_fSpeed = 150.f;
		m_bDead = false;
		m_iDmg = 10;

		_vec3 vScale = { 0.025f, 0.025f, 0.025f };
		m_pTransformCom->Set_Scale(vScale);
	}

	return S_OK;
}

_int CShotgunBullet::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return -1;

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	if (-1 == CBullet::Update_GameObject(fTimeDelta))
		return -1;

	Linear_Movement(fTimeDelta);
	Bill_Board();

	return 0;
}

void CShotgunBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);
	m_pColliderCom->Late_Update();
}

void CShotgunBullet::Render_GameObject(const _float& fTimeDelta)
{
	CBullet::Render_GameObject(fTimeDelta);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CShotgunBullet::OnCollision(CCollider* _pOther)
{
	if (_pOther->Get_ColLayer() == COLLAYERID::COL_PLAYER)
		return;
}

void CShotgunBullet::OnCollisionEnter(CCollider* _pOther)
{
	switch (_pOther->Get_ColLayer())
	{
	case COLLAYERID::COL_PLAYER:
		return;

	case COLLAYERID::COL_TERRAIN:
	case COLLAYERID::COL_WALL:
		m_fSpeed = 0.f;
		break;

	case COLLAYERID::COL_ENEMY:
		m_fSpeed = 0.f;
		_pOther->Get_Owner()->Take_Dmg(m_iDmg);
		break;

	default:
		break;
	}

	m_bDead = true;
}

void CShotgunBullet::OnCollisionExit(CCollider* _pOther)
{
	if (_pOther->Get_ColLayer() == COLLAYERID::COL_PLAYER)
		return;
}

HRESULT CShotgunBullet::Add_Component()
{
	// 부모에서 Buffer, Transform, Collider, Animation 컴포넌트 추가 후 ID_DYNAMIC에 삽입
	CBullet::Add_Component();

	// ShotgunBullet 만의 텍스쳐 추가
	CComponent* pComponent = nullptr;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_NormalBullet"));

	if (pComponent == nullptr)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CShotgunBullet* CShotgunBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShotgunBullet* pShotgunBullet = new CShotgunBullet(pGraphicDev);

	return pShotgunBullet;
}

CShotgunBullet* CShotgunBullet::Clone()
{
	CShotgunBullet* pBullet = new CShotgunBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return nullptr;
	}

	return pBullet;
}

CShotgunBullet* CShotgunBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CShotgunBullet* pBullet = new CShotgunBullet(*this);

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