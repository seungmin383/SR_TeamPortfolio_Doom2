#include "pch.h"
#include "CPinkyBullet.h"
#include "CImp.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CDynamicCamera.h"
#include "CMonsterBase.h"

CPinkyBullet::CPinkyBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBullet(pGraphicDev)
{
}

CPinkyBullet::CPinkyBullet(const CPinkyBullet& rhs)
	:CBullet(rhs)
{
}

CPinkyBullet::~CPinkyBullet()
{
	Free();
}

HRESULT CPinkyBullet::Ready_GameObject()
{
	//AllocConsole();

	//FILE* fp;
	//freopen_s(&fp, "CONOUT$", "w", stdout);

	if (FAILED(Add_Component()))
	{
		return E_FAIL;
	}

	m_pPlayer = CManagement::GetInstance()->Get_Player();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(2.f, 2.f, 2.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_BULLET,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT);

	if (m_pOwner != nullptr)
	{
		m_vInitDir = dynamic_cast<CMonsterBase*>(m_pOwner)->Get_MoveLook();
		D3DXVec3Normalize(&m_vInitDir, &m_vInitDir);
		m_fSpeed = 15.f;
		m_bDead = FALSE;
		m_iDmg = dynamic_cast<CMonsterBase*>(m_pOwner)->GetAtkDmg();
	}


	return S_OK;
}

_int CPinkyBullet::Update_GameObject(const _float& fTimeDelta)
{

	Calc_Time_EndAttack(fTimeDelta);

	CBullet::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	_vec3 vPos;
	if (m_pOwner != nullptr)
	{
		m_pOwner->Get_Transform()->Get_Pos(&vPos);
		_vec3 vDir = dynamic_cast<CMonsterBase*>(m_pOwner)->Get_MoveLook();
		D3DXVec3Normalize(&vDir, &vDir);
		vPos += vDir * 2;
		m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	}


	if (m_bDead)
	{
		m_bDead = FALSE;
		return -1;
	}

	Bill_Board();
	return 0;
}

void CPinkyBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);

	m_pColliderCom->Late_Update();
}

void CPinkyBullet::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	CBullet::Render_GameObject(fTimeDelta);


	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CPinkyBullet::Check_Anim_Is_Finish()
{
	if (Enter_Func_Cnt == 0)									//이 함수에 처음 입장했다면
	{
		Enter_Func_Cnt++;										//카운트만 올리고 return 
		isFinAnim = FALSE;
		return;
	}
	else
	{
		if (m_pAnimCom->Get_isFinAct())			//애니메이션이 끝났다면
		{
			Enter_Func_Cnt = 0;
			isFinAnim = TRUE;
			m_isEverEndAinm = TRUE;
			return;
		}
		else
		{
			Enter_Func_Cnt++;
			return;
		}
	}
}

void CPinkyBullet::Calc_Time_EndAttack(const _float& fTimeDelta)
{
	m_fTimer += fTimeDelta;

	if (m_fTimer > 1.f)
	{
		m_fTimer = 0.f;
		m_bDead = TRUE;
	}
}


HRESULT CPinkyBullet::Add_Component()
{
	CBullet::Add_Component();

	CComponent* pComponent = NULL;
	//texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Zombieman_Bullet_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture1", pComponent });
	return S_OK;
}

CPinkyBullet* CPinkyBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPinkyBullet* Bullet = new CPinkyBullet(pGraphicDev);

	return Bullet;
}


CPinkyBullet* CPinkyBullet::Clone()
{

	CPinkyBullet* pBullet = new CPinkyBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return	nullptr;
	}

	return pBullet;
}

CPinkyBullet* CPinkyBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CPinkyBullet* pBullet = new CPinkyBullet(*this);

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

void CPinkyBullet::Free()
{
	CBullet::Free();
}
