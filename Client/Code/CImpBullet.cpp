#include "pch.h"
#include "CImpBullet.h"
#include "CImp.h"
#include "CRenderer.h"

CImpBullet::CImpBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBullet(pGraphicDev)
{
}

CImpBullet::CImpBullet(const CImpBullet& rhs)
	:CBullet(rhs)
{
}

CImpBullet::~CImpBullet()
{
	Free();
}

HRESULT CImpBullet::Ready_GameObject()
{
	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	if (FAILED(Add_Component()))
	{
		return E_FAIL;
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_BULLET,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT );

	if (m_pOwner != nullptr)
	{
		m_vInitDir = dynamic_cast<CMonsterBase*>(m_pOwner)->Get_MoveLook();
		D3DXVec3Normalize(&m_vInitDir, &m_vInitDir);
		m_fSpeed = 15.f;
		m_bDead = FALSE;
		m_iDmg = dynamic_cast<CMonsterBase*>(m_pOwner)->GetAtkDmg();
	}

	//날라갈 때
	m_pAnimCom->Add_Animation(
		0,
		m_pTextureCom,
		m_pBufferCom,
		2,
		1,
		0,
		1,
		0.15f,
		true,
		false
	);

	//터질 때
	m_pAnimCom->Add_Animation(
		1,
		m_pTextureCom,
		m_pBufferCom,
		3,
		2,
		0,
		2,
		0.15f,
		false,
		false
	);

	return S_OK;
}

_int CImpBullet::Update_GameObject(const _float& fTimeDelta)
{
	if (GetAsyncKeyState('H'))
		m_bDead = TRUE;
	Check_Anim_Is_Finish();

	CBullet::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	if(!m_bDead)
		Linear_Movement(fTimeDelta);

	m_pColliderCom->Late_Update();
	Bill_Board();
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	cout << "아" << endl;
 
	if (m_bDead && isFinAnim)
	{
		m_bDead = FALSE;
		return -1;
	}

	return 0;
}

void CImpBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);

	if(m_bDead)
		m_pAnimCom->Play_Animation(1, 0, fTimeDelta, 0);
	else
		m_pAnimCom->Play_Animation(0, 0, fTimeDelta, 0);
}

void CImpBullet::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());	
	CBullet::Render_GameObject(fTimeDelta);

	if(m_bDead)
		m_pTextureCom->Set_Texture(1);
	else
		m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CImpBullet::Check_Anim_Is_Finish()
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
			return ;
		}
		else
		{
			Enter_Func_Cnt++;
			return;
		}
	}
}

HRESULT CImpBullet::Add_Component()
{
	CBullet::Add_Component();

	CComponent* pComponent = NULL;
	//texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Imp_Bullet_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
}

CImpBullet* CImpBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CImpBullet* Bullet = new CImpBullet(pGraphicDev);

	return Bullet;
}


CImpBullet* CImpBullet::Clone()
{

	CImpBullet* pBullet = new CImpBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return	nullptr;
	}

	return pBullet;
}

CImpBullet* CImpBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CImpBullet* pBullet = new CImpBullet(*this);

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

void CImpBullet::Free()
{
	CBullet::Free();
}
