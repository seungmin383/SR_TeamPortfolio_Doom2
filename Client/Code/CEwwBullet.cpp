#include "pch.h"
#include "CEwwBullet.h"
#include "CImp.h"
#include "CRenderer.h"
#include "CCollisionManager.h"

CEwwBullet::CEwwBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBullet(pGraphicDev)
{
}

CEwwBullet::CEwwBullet(const CEwwBullet& rhs)
	:CBullet(rhs)
{
}

CEwwBullet::~CEwwBullet()
{
	Free();
}

HRESULT CEwwBullet::Ready_GameObject()
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
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(5.f, 5.f, 5.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_BULLET,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT | COLLAYERID::COL_ENEMY);

	if (m_pOwner != nullptr)
	{
		m_vInitDir = dynamic_cast<CMonsterBase*>(m_pOwner)->Get_MoveLook();
		D3DXVec3Normalize(&m_vInitDir, &m_vInitDir);
		m_fSpeed = 150.f;
		m_bDead = FALSE;
		m_iDmg = dynamic_cast<CMonsterBase*>(m_pOwner)->GetAtkDmg();
		m_pTransformCom->Set_Scale(_vec3{ 5.f, 5.f, 5.f });
	}

	//날라갈 때
	m_pAnimCom->Add_Animation(
		0,
		m_pTextureCom,
		m_pBufferCom,
		5,
		1,
		0,
		4,
		0.15f,
		true,
		false
	);


	return S_OK;
}

_int CEwwBullet::Update_GameObject(const _float& fTimeDelta)
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

	if (CBullet::Update_GameObject(fTimeDelta) == -1)
		return -1;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	if (!m_bDead)
		Linear_Movement(fTimeDelta);

	Bill_Board();
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	return 0;
}

void CEwwBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);

		m_pAnimCom->Play_Animation(0, 0, fTimeDelta, 0);

	m_pColliderCom->Late_Update();
}

void CEwwBullet::Render_GameObject(const _float& fTimeDelta)
{
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	LONG minx = min(pos.x + 5, pos.x - 5);
	LONG maxx = max(pos.x + 5, pos.x - 5);
	LONG minz = min(pos.z + 5, pos.z - 5);
	LONG maxz = max(pos.z + 5, pos.z - 5);

	RECT myRect{ minx, minz, maxx, maxz };
	if (CCollisionManager::Check_AABB(myRect) == false)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


		m_pTextureCom->Set_Texture(0);

	CBullet::Render_GameObject(fTimeDelta);


	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
}

void CEwwBullet::Check_Anim_Is_Finish()
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

HRESULT CEwwBullet::Add_Component()
{
	CBullet::Add_Component();

	CComponent* pComponent = NULL;
	//texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"eww"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
}

CEwwBullet* CEwwBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEwwBullet* Bullet = new CEwwBullet(pGraphicDev);

	return Bullet;
}


CEwwBullet* CEwwBullet::Clone()
{

	CEwwBullet* pBullet = new CEwwBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return	nullptr;
	}

	return pBullet;
}

CEwwBullet* CEwwBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CEwwBullet* pBullet = new CEwwBullet(*this);

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

void CEwwBullet::Free()
{
	CBullet::Free();
}
