#include "pch.h"
#include "Engine_Enum.h"
#include "CRocketBullet.h"
#include "CManagement.h"
#include "CDynamicCamera.h"
#include "CRenderer.h"

CRocketBullet::CRocketBullet(LPDIRECT3DDEVICE9 pGraphicDev) :
	CBullet(pGraphicDev)
{
}

CRocketBullet::CRocketBullet(const CRocketBullet& rhs) :
	CBullet(rhs)
{
}

CRocketBullet::~CRocketBullet()
{
	Free();
}

HRESULT CRocketBullet::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	// Set_GameLogicObject 할 때 포지션을 넘겨준다.
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	// Collider Settings
	_vec3 vColliderScale = { 0.5f, 0.5f, 0.5f };
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
		m_fSpeed = 80.f;
		m_bDead = false;
		m_iDmg = 100;

		_vec3 vScale = { 0.35f, 0.35f, 0.35f };
		m_pTransformCom->Set_Scale(vScale);
	}

	return S_OK;
}

_int CRocketBullet::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead && m_pAnimCom->Get_isFinAct())
		return -1;

	if (CManagement::GetInstance()->Is_Paused())			// 여기서 일시 정지를 하면 멈추긴 하는데, Texture가 안보임
		return 0;

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	if (CBullet::Update_GameObject(fTimeDelta) == -1)
		return -1;

	m_iAnimState = Calc_AnimState_byPlayer(m_vInitDir, vPos);

	Linear_Movement(fTimeDelta);
	Bill_Board();

	return 0;
}

void CRocketBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_eState == BULLET_STATE::HIT)
	{
		// 충돌 후에는 텍스쳐와 콜라이더 스케일을 크게 만든다.
		_vec3 vAfterScale = { 2.f, 2.f, 2.f };
		m_pTransformCom->Set_Scale(vAfterScale);
		_vec3 vAfterScale1 = { 8.f, 8.f, 8.f };
		m_pColliderCom->Set_Scale(vAfterScale1);

		m_iAnimState = 8;
		m_pAnimCom->Play_Animation((_uint)BULLET_STATE::HIT, 0, fTimeDelta);
		
		if (m_pAnimCom->Get_isFinAct())
			m_bDead = true;
	}

	CBullet::LateUpdate_GameObject(fTimeDelta);
	m_pColliderCom->Late_Update();
}

void CRocketBullet::Render_GameObject(const _float& fTimeDelta)
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

void CRocketBullet::OnCollision(CCollider* _pOther)
{
	if (_pOther->Get_ColLayer() == COLLAYERID::COL_ENEMY)
	{
		_pOther->Get_Owner()->Take_Dmg(m_iDmg);
		m_bDead = true;
	}
}

void CRocketBullet::OnCollisionEnter(CCollider* _pOther)
{
	switch (_pOther->Get_ColLayer())
	{
	case COLLAYERID::COL_PLAYER:
		return;

	case COLLAYERID::COL_OBJECT:
	case COLLAYERID::COL_ENEMY:
		m_fSpeed = 0.f;
		m_eState = BULLET_STATE::HIT;
		_pOther->Get_Owner()->Take_Dmg(m_iDmg);
		break;

	default:
		break;
	}
}

void CRocketBullet::OnCollisionExit(CCollider* _pOther)
{
	if (_pOther->Get_ColLayer() == COLLAYERID::COL_PLAYER)
		return;
}

HRESULT CRocketBullet::Add_Component()
{
	// 부모에서 Buffer, Transform, Collider, Animation 컴포넌트 추가 후 ID_DYNAMIC에 삽입
	CBullet::Add_Component();

	// RocketBullet 만의 텍스쳐 추가
	CComponent* pComponent = nullptr;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RocketBullet"));

	if (pComponent == nullptr)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CRocketBullet::Ready_Animation()
{
	// 폭발 시의 애니메이션
	m_pAnimCom->Add_Animation((_uint)BULLET_STATE::HIT, m_pTextureCom, m_pBufferCom, 3, 1, 0, 2, 0.15f, false, false);
}

CRocketBullet* CRocketBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRocketBullet* pRocketBullet = new CRocketBullet(pGraphicDev);

	return pRocketBullet;
}

//플레이어와 몬스터의 각도를 구해주는 함수
_float CRocketBullet::Calc_AngleToPlayer(_vec3 vLook, _vec3 vPos)
{
	// 플레이어의 위치를 가져온다.
	_vec3 vPlayerPos;
	CManagement::GetInstance()->Get_Player()->Get_Transform()->Get_Pos(&vPlayerPos);

	//플레이어가 몬스터를 바라보는 방향의 벡터를 구함
	_vec3 PlayerLook = vPlayerPos - vPos;

	//정규화 해주고
	D3DXVec3Normalize(&PlayerLook, &PlayerLook);
	D3DXVec3Normalize(&vLook, &vLook);

	float angleLook = atan2f(vLook.z, vLook.x);
	float angleToPlayer = atan2f(PlayerLook.z, PlayerLook.x);

	float fAngle = D3DXToDegree(angleToPlayer - angleLook);

	if (fAngle > 180.f)
		fAngle -= 360.f;
	if (fAngle < -180.f)
		fAngle += 360.f;

	return fAngle;
}


//플레이어와 총알의 각도를 이용해서 로켓이 보여지는 애니메이션 상태를 정해줌
_int CRocketBullet::Calc_AnimState_byPlayer(_vec3 vLook, _vec3 vPos)
{
	_float fAngle = Calc_AngleToPlayer(vLook, vPos);

	if (fAngle >= 0.f && fAngle < 30.f)
		return 4;
	else if (fAngle >= 30.f && fAngle < 60.f)
		return 5;
	else if (fAngle >= 60.f && fAngle < 90.f)
		return 6;
	else if (fAngle >= 90.f && fAngle < 120.f)
		return 6;
	else if (fAngle >= 120.f && fAngle < 150.f)
		return 7;
	else if (fAngle >= 150.f && fAngle <= 180.f)
		return 0;

	else if (fAngle < 0.f && fAngle >= -30.f)
		return 4;
	else if (fAngle < -30.f && fAngle >= -60.f)
		return 3;
	else if (fAngle < -60.f && fAngle >= -90.f)
		return 2;
	else if (fAngle < -90.f && fAngle >= -120.f)
		return 2;
	else if (fAngle < -120.f && fAngle >= -150.f)
		return 1;
	else if (fAngle < -150.f && fAngle >= -180.f)
		return 0;
}


CRocketBullet* CRocketBullet::Clone()
{
	CRocketBullet* pBullet = new CRocketBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return nullptr;
	}

	return pBullet;
}

CRocketBullet* CRocketBullet::Clone(CGameObject* _Owner)
{
	CGameObject::Clone(_Owner);

	CRocketBullet* pBullet = new CRocketBullet(*this);

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