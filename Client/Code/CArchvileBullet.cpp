#include "pch.h"
#include "CArchvileBullet.h"
#include "CImp.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CDynamicCamera.h"
#include "CMonsterBase.h"

CArchvileBullet::CArchvileBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBullet(pGraphicDev)
{
}

CArchvileBullet::CArchvileBullet(const CArchvileBullet& rhs)
	:CBullet(rhs)
{
}

CArchvileBullet::~CArchvileBullet()
{
	Free();
}

HRESULT CArchvileBullet::Ready_GameObject()
{
	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	if (FAILED(Add_Component()))
	{
		return E_FAIL;
	}

	m_pPlayer = CManagement::GetInstance()->Get_Player();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f),
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

	//날라갈 때
	m_pAnimCom->Add_Animation(
		0,
		m_pTextureCom,
		m_pBufferCom,
		17,
		1,
		0,
		16,
		0.2f,
		true,
		false
	);

	return S_OK;
}

_int CArchvileBullet::Update_GameObject(const _float& fTimeDelta)
{
	if (GetAsyncKeyState('H'))
		m_bDead = TRUE;
	Check_Anim_Is_Finish();

	CBullet::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_pOwner != nullptr && dynamic_cast<CMonsterBase*>(m_pOwner)->Get_AttackTarget() == nullptr) //플레이어
	{
		_vec3 vPlayerPos;
		m_pPlayer->Get_Transform()->Get_Pos(&vPlayerPos);

		D3DXMATRIX camera;
		D3DXMatrixIdentity(&camera);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &camera);
		D3DXMatrixInverse(&camera, NULL, &camera);
		_vec3 vPlayerLook;
		vPlayerLook.x = camera._31;
		vPlayerLook.y = camera._32;
		vPlayerLook.z = camera._33;

		LockOn_Movement(fTimeDelta, vPlayerPos, vPlayerLook, 2);
	}
	else if(m_pOwner != nullptr)
	{
		_vec3 vTargetPos;
		dynamic_cast<CMonsterBase*>(m_pOwner)->Get_AttackTarget()->Get_Transform()->Get_Pos(&vTargetPos);
		//LockOn_Movement(fTimeDelta, vTargetPos);
	}

	Bill_Board();
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	cout <<"X: " << vPos.x << " Y : " << vPos.y << " Z : " << vPos.z << endl;

	if (m_bDead && isFinAnim)
	{
		m_bDead = FALSE;
		return -1;
	}

	return 0;
}

void CArchvileBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);

	m_pAnimCom->Play_Animation(0, 0, fTimeDelta, 0);
	m_pColliderCom->Late_Update();
}

void CArchvileBullet::Render_GameObject(const _float& fTimeDelta)
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

void CArchvileBullet::Check_Anim_Is_Finish()
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
			return;
		}
		else
		{
			Enter_Func_Cnt++;
			return;
		}
	}
}


HRESULT CArchvileBullet::Add_Component()
{
	CBullet::Add_Component();

	CComponent* pComponent = NULL;
	//texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Archvile_Bullet_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
	return S_OK;
}

CArchvileBullet* CArchvileBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CArchvileBullet* Bullet = new CArchvileBullet(pGraphicDev);

	return Bullet;
}


CArchvileBullet* CArchvileBullet::Clone()
{

	CArchvileBullet* pBullet = new CArchvileBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return	nullptr;
	}

	return pBullet;
}

CArchvileBullet* CArchvileBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CArchvileBullet* pBullet = new CArchvileBullet(*this);

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

void CArchvileBullet::Free()
{
	CBullet::Free();
}
