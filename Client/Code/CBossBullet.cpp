#include "pch.h"
#include "CBossBullet.h"
#include "CImp.h"
#include "CRenderer.h"
#include"CSoundManager.h"

CBossBullet::CBossBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBullet(pGraphicDev)
{
}

CBossBullet::CBossBullet(const CBossBullet& rhs)
	:CBullet(rhs)
{
}

CBossBullet::~CBossBullet()
{
	Free();
}

HRESULT CBossBullet::Ready_GameObject()
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
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(30.f, 30.f, 30.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_BULLET,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT | COLLAYERID::COL_ENEMY);

	if (m_pOwner != nullptr)
	{
		m_vInitDir = dynamic_cast<CMonsterBase*>(m_pOwner)->Get_MoveLook();
		D3DXVec3Normalize(&m_vInitDir, &m_vInitDir);
		m_fSpeed = 100.f;
		m_bDead = FALSE;
		m_iDmg = dynamic_cast<CMonsterBase*>(m_pOwner)->GetAtkDmg();
		m_pTransformCom->Set_Scale(_vec3{ 30.f, 30.f, 30.f });
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
		5,
		1,
		0,
		4,
		0.15f,
		false,
		false
	);

	CSoundManager::GetInstance()->PlaySoundW(L"DSFLAME.wav", SOUND_EFFECT, 0.6f);
	return S_OK;
}

_int CBossBullet::Update_GameObject(const _float& fTimeDelta)
{
	if (GetAsyncKeyState('H'))
		m_bDead = TRUE;

	if (m_bDead || m_eState == BULLET_STATE::HIT)
		isDead = TRUE;

	Check_Anim_Is_Finish();
	if (isDead && isFinAnim)
	{
		m_bDead = FALSE;
		isDead = FALSE;
		return -1;
	}
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	if (CBullet::Update_GameObject(fTimeDelta) == -1)
	{
		CSoundManager::GetInstance()->PlaySound3D(L"EXPLOSION", vPos, 1.f);
		return -1;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	if (!m_bDead)
		Linear_Movement(fTimeDelta);

	Bill_Board();


	return 0;
}

void CBossBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);

	if (isDead)
		m_pAnimCom->Play_Animation(1, 0, fTimeDelta, 0);
	else
		m_pAnimCom->Play_Animation(0, 0, fTimeDelta, 0);

	m_pColliderCom->Late_Update();
}

void CBossBullet::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (isDead)
		m_pTextureCom->Set_Texture(1);
	else
		m_pTextureCom->Set_Texture(0);

	CBullet::Render_GameObject(fTimeDelta);


	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CBossBullet::Check_Anim_Is_Finish()
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

HRESULT CBossBullet::Add_Component()
{
	CBullet::Add_Component();

	CComponent* pComponent = NULL;
	//texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Boss_Bullet_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
}

CBossBullet* CBossBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossBullet* Bullet = new CBossBullet(pGraphicDev);

	return Bullet;
}


CBossBullet* CBossBullet::Clone()
{

	CBossBullet* pBullet = new CBossBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return	nullptr;
	}

	return pBullet;
}

CBossBullet* CBossBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CBossBullet* pBullet = new CBossBullet(*this);

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

void CBossBullet::Free()
{
	CBullet::Free();
}
