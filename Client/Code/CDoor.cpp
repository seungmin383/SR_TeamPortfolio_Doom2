#include "pch.h"
#include "CDoor.h"
#include "CBarrelExplosive.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CTerrainSector.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CDoor::CDoor(const CGameObject& rhs)
	:CGameObject(rhs)
{
}

CDoor::~CDoor()
{
}

HRESULT CDoor::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSpeed = 5.f;
	m_fTimer = 0.f;

	m_pTransformCom->Set_Pos(20.f, 3.f, 25.f);					// Scale.y 값이랑 y 값이랑 같게 해야 한다.
	m_pTransformCom->Set_Scale(_vec3(3.f, 3.f, 0.5f));		

	m_pTransformCom->Get_Pos(&m_vOriginPos);

	// Collider 세팅
	if (m_pColliderCom)
	{
		m_pColliderCom->Ready_CCollider(
			this,
			_vec3(0, 0, 0),         // 초기 위치(Transform가 실제로 잡아줌)
			_vec3(3.f, 3.f, 1.5f),   // 박스 크기
			_vec3(0, 0.f, 0),      // 약간 위로
			COL_OBJECT,             // 내 레이어
			// 공격으로 인정할 대상 레이어(레이 사격 포함)
			COL_PLAYER
		);
		m_pColliderCom->Set_Active(true);
	}

	if (m_state == State::Open)
		m_fTimer++;
	else
		m_fTimer = 0;

	m_fOpenHeight = m_vOriginPos.y * 2;

	return S_OK;
}

_int CDoor::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	DoorOpening(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}

void CDoor::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // ★중요
}

void CDoor::Render_GameObject(const _float& fTimeDelta)
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

void CDoor::OnCollision(CCollider* _pOther)
{
}

void CDoor::OnCollisionEnter(CCollider* _pOther)
{
	if (!_pOther) return;

	if (m_state == State::Idle)
	{
		const auto layer = _pOther->Get_ColLayer();
		const bool allowed = layer & (COL_PLAYER);
		// 플레이어랑만 처리
		if (allowed) {
			OpenDoor();
		}
	}	
}

void CDoor::OnCollisionExit(CCollider* _pOther)
{
}

void CDoor::OpenDoor()
{
	if (m_state == State::Idle)
	{
		m_state = State::Opening;
	}
}

HRESULT CDoor::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_CubeTex"));
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

void CDoor::Set_OnTerrain()
{
	for (auto terrain : CManagement::GetInstance()->Get_Scene()->Get_Terrain())
	{
		_vec3 pos;
		m_pTransformCom->Get_Pos(&pos);

		auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();

		_vec3 onPosition{ 0.f, 0.f, 0.f };
		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &pos))
		{

			m_pTransformCom->Set_Pos(pos.x, onPosition.y, pos.z);
			break;
		}
	}
}

void CDoor::Bill_Board()
{
}

void CDoor::DoorOpening(const _float& fTimeDelta)
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
			// XZ는 유지, Y만 원래 높이에 OpenHeight만큼 증가
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

CDoor* CDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDoor* pDoor = new CDoor(pGraphicDev);

	return pDoor;
}

CDoor* CDoor::Clone()
{
	CDoor* pDoor = new CDoor(*this);

	if (FAILED(pDoor->Ready_GameObject()))
	{
		Safe_Release(pDoor);
		MSG_BOX("Door Create Failed");
		return nullptr;
	}

	return pDoor;
}

void CDoor::Free()
{
	Engine::CGameObject::Free();
}
