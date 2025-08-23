#include "pch.h"
#include "CDoorWithKey.h"
#include "CBarrelExplosive.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"

CDoorWithKey::CDoorWithKey(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CDoorWithKey::CDoorWithKey(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CDoorWithKey::~CDoorWithKey()
{
}

HRESULT CDoorWithKey::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSpeed = 5.f;
	m_fTimer = 0.f;

	m_pTransformCom->Set_Pos(20.f, 2.f, 25.f);
	m_pTransformCom->Set_Scale(_vec3(2.f, 2.f, 2.f));

	m_pTransformCom->Get_Pos(&m_vOriginPos);

	// Collider 세팅
	if (m_pColliderCom)
	{
		m_pColliderCom->Ready_CCollider(
			this,
			_vec3(0, 0, 0),         // 초기 위치(Transform가 실제로 잡아줌)
			_vec3(2.f, 2.f, 1.f),   // 박스 크기
			_vec3(0, 0.4f, 0),      // 약간 위로
			COL_OBJECT,             // 내 레이어
			// 공격으로 인정할 대상 레이어(레이 사격 포함)
			COL_PLAYERCAMERA
		);
		m_pColliderCom->Set_Active(true);
	}

	if (m_state == State::Open)
		m_fTimer++;
	else
		m_fTimer = 0;

	return S_OK;
}

_int CDoorWithKey::Update_GameObject(const _float& fTimeDelta)
{
	if (!CManagement::GetInstance()->Is_KeyOwned()) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
		return 0;
	}

	CGameObject::Update_GameObject(fTimeDelta);

	DoorOpening(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CDoorWithKey::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // 중요
}

void CDoorWithKey::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CDoorWithKey::OnCollision(CCollider* _pOther)
{
}

void CDoorWithKey::OnCollisionEnter(CCollider* _pOther)
{
	if (!_pOther) return;

	if (m_state == State::Idle)
	{
		const auto layer = _pOther->Get_ColLayer();
		const bool allowed = layer & (COL_PLAYER);
		// 플레이어랑만 처리
		if (!allowed) {
			OpenDoor();
		}
	}
}

void CDoorWithKey::OnCollisionExit(CCollider* _pOther)
{
}

void CDoorWithKey::OpenDoor()
{
	if (m_state == State::Idle)
	{
		m_state = State::Opening;
	}
}

HRESULT CDoorWithKey::Add_Component()
{
	CComponent* pComponent = NULL;

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

	//Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Door_1"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	OutputDebugStringA("[Stage] add OBJ_ITEM: Door\n");

	return S_OK;
}

void CDoorWithKey::Set_OnTerrain()
{
	if (!m_pTransformCom || !m_pCalculatorCom) return;

	auto* pTerrainBuf = static_cast<CTerrainTex*>(
		CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));
	if (!pTerrainBuf) return;

	_vec3 pos; m_pTransformCom->Get_Pos(&pos);

	// 그리드 경계(0..(VTXCNT-1)*VTXITV) 밖이면 보정 건너뜀 (혹은 clamp)
	const float maxX = (VTXCNTX - 1) * VTXITV - 1e-3f;
	const float maxZ = (VTXCNTZ - 1) * VTXITV - 1e-3f;
	if (pos.x < 0.f || pos.z < 0.f || pos.x > maxX || pos.z > maxZ) return;

	float h = m_pCalculatorCom->Compute_HeightOnTerrain(&pos, pTerrainBuf->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	// 살짝 띄워서 지면 간섭 방지
	m_pTransformCom->Set_Pos(pos.x, h + 1.f, pos.z);
}

void CDoorWithKey::Bill_Board()
{
}

void CDoorWithKey::DoorOpening(const _float& fTimeDelta)
{
	_vec3 vUp = { 0.f, 1.f, 0.f };
	_vec3 vDown = { 0.f, -1.f, 0.f };

	switch (m_state)
	{
	case State::Opening:
	{
		_vec3 vPos;
		m_pTransformCom->Get_Pos(&vPos);

		if (vPos.y >= m_vOriginPos.y + m_fOpenHeight)
		{
			vPos.y = m_vOriginPos.y + m_fOpenHeight;
			m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
			m_state = State::Open;
			m_fTimer = 0.f;
		}
		else
		{
			m_pTransformCom->Move_Pos(&vUp, m_fSpeed, fTimeDelta);
		}
		break;
	}

	case State::Open:
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer >= m_fStayOpenTime)
		{
			m_state = State::Closing;
		}
		break;
	}
	case State::Closing:
	{
		_vec3 vPos;
		m_pTransformCom->Get_Pos(&vPos);

		if (vPos.y <= m_vOriginPos.y)
		{
			// XZ는 유지, Y만 원래 높이로
			vPos.y = m_vOriginPos.y;
			m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
			m_state = State::Idle;
		}
		else
		{
			m_pTransformCom->Move_Pos(&vDown, m_fSpeed, fTimeDelta);
		}
		break;
	}
	}
}

CDoorWithKey* CDoorWithKey::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDoorWithKey* pDoor = new CDoorWithKey(pGraphicDev);

	return pDoor;
}

CDoorWithKey* CDoorWithKey::Clone()
{
	CDoorWithKey* pDoor = new CDoorWithKey(*this);

	if (FAILED(pDoor->Ready_GameObject()))
	{
		Safe_Release(pDoor);
		MSG_BOX("Door Create Failed");
		return nullptr;
	}

	return pDoor;
}

void CDoorWithKey::Free()
{
	Engine::CGameObject::Free();
}
