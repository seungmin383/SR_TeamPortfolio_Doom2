#include "pch.h"
#include "CZombiemanBullet.h"
#include "CZombieMan.h"
#include "CRenderer.h"

CZombiemanBullet::CZombiemanBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBullet(pGraphicDev)
{
}

CZombiemanBullet::CZombiemanBullet(const CZombiemanBullet& rhs)
	:CBullet(rhs)
{
}

CZombiemanBullet::~CZombiemanBullet()
{
	Free();
}

HRESULT CZombiemanBullet::Ready_GameObject()
{
	//AllocConsole();

	//FILE* fp;
	//freopen_s(&fp, "CONOUT$", "w", stdout);
	//freopen_s(&fp, "CONOUT$", "w", stderr);

	if (FAILED(Add_Component()))
	{
		return E_FAIL;
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(0.1f, 0.1f, 0.1f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_BULLET,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT | COLLAYERID::COL_ENEMY);

	if (m_pOwner != nullptr)
	{
		m_vInitDir = dynamic_cast<CMonsterBase*>(m_pOwner)->Get_MoveLook();
		D3DXVec3Normalize(&m_vInitDir, &m_vInitDir);
		m_fSpeed = 100.f;
		m_bDead = FALSE;
		m_iDmg = dynamic_cast<CMonsterBase*>(m_pOwner)->GetAtkDmg();
		m_pTransformCom->Set_Scale(_vec3{ 0.05f, 0.05f, 0.05f });
	}

	return S_OK;
}

_int CZombiemanBullet::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		isDead = TRUE;

	if (isDead)
	{
		m_bDead = FALSE;
		isDead = FALSE;
		return -1;
	}

	if (CBullet::Update_GameObject(fTimeDelta) == -1)
		return -1;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (!m_bDead)
		Linear_Movement(fTimeDelta);

	Bill_Board();

	return 0;
}

void CZombiemanBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);

//	m_pAnimCom->Play_Animation(0, 0, fTimeDelta, 0);
	m_pColliderCom->Late_Update();
}

void CZombiemanBullet::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);
	CBullet::Render_GameObject(fTimeDelta);


	m_pBufferCom->Render_Buffer();

	//�浹ü
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

HRESULT CZombiemanBullet::Add_Component()
{
	CBullet::Add_Component();
	//texture
	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Zombieman_Bullet_Texture"));
	
	if (nullptr == pComponent)
		return E_FAIL;

	return S_OK;
}

CZombiemanBullet* CZombiemanBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CZombiemanBullet* Bullet = new CZombiemanBullet(pGraphicDev);

	return Bullet;
}


CZombiemanBullet* CZombiemanBullet::Clone()
{

	CZombiemanBullet* pBullet = new CZombiemanBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return	nullptr;
	}

	return pBullet;
}

CZombiemanBullet* CZombiemanBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CZombiemanBullet* pBullet = new CZombiemanBullet(*this);

	if (pBullet == nullptr)
		return nullptr;

	pBullet->m_pOwner = _Owner;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return	nullptr;
	}

	return pBullet;
}

void CZombiemanBullet::Free()
{
	Safe_Release(m_pTextureCom);

	CBullet::Free();
}
