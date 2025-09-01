#include "pch.h"
#include "CSwitch_Button.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CStage.h"
#include "CStage2.h"
#include "CStage3.h"
#include "CTerrainSector.h"
#include "CGameObject.h"

CSwitch_Button::CSwitch_Button(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_pObjTarget(nullptr)
{
}

CSwitch_Button::CSwitch_Button(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CSwitch_Button::~CSwitch_Button()
{
}

HRESULT CSwitch_Button::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_bPressed = false;
    m_state    = State::Idle;   // Idle로 초기화

	m_pTransformCom->Set_Pos(15.f, 2.f, 18.f);
	m_pTransformCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
		_vec3(0, 0.4f, 0),        // 약간 위로
		COL_BUTTON,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYER       // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	return S_OK;
}

_int CSwitch_Button::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Set_OnTerrain();
	Bill_Board();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CSwitch_Button::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // 중요
}

void CSwitch_Button::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (m_bPressed) {
		m_pTextureCom->Set_Texture();
	}
	else
	{
		m_pTextureCom2->Set_Texture();
	}


	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CSwitch_Button::OnCollision(CCollider* _pOther)
{
	if (!_pOther) return;
	// 레이어(마스크) 체크
	if ((_pOther->Get_ColLayer() & COLLAYERID::COL_PLAYER) == 0) return;

	// 안전 캐스팅
	auto* cam = dynamic_cast<CDynamicCamera*>(_pOther->Get_Owner());
	if (!cam) return;

	if (cam->Get_Trigger()) {
		m_bPressed = true;
		SetSwitchOn();
		m_state = State::Dead;
	}
}

void CSwitch_Button::OnCollisionEnter(CCollider* _pOther)
{
	if (!_pOther) return;
}

void CSwitch_Button::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CSwitch_Button::Add_Component()
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
	m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Switch_On"));
	if (nullptr == m_pTextureCom)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_On", m_pTextureCom });

	m_pTextureCom2 = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Switch_Off"));
	if (nullptr == m_pTextureCom2)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Off", m_pTextureCom2 });

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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Switch\n");

	return S_OK;
}

void CSwitch_Button::Set_OnTerrain()
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

			m_pTransformCom->Set_Pos(pos.x, onPosition.y + 1, pos.z);
			break;
		}
	}
}

void CSwitch_Button::Bill_Board()
{
	if (!m_pTransformCom)
		return;

	_matrix matWorld, matView, matBill;
	m_pTransformCom->Get_World(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);		// 카메라가 갖고 있는 y회전의 역행렬 -역행렬> 카메라의 월드 상태의 y축 회전 행렬

	matWorld = matBill * matWorld;
	// 최종적으로 빌보드 행렬을 월드 행렬로 설정                                           
	m_pTransformCom->Set_World(&matWorld);
}

void CSwitch_Button::SetSwitchOn()
{
	if (m_linkMode == Engine::LINKMODE::ONE_TO_MANY) {
		for (auto* d : m_targets) if (d) d->OpenDoor();  // 1:N
	}
	else if(m_linkMode == Engine::LINKMODE::ONE_TO_ONE){
		CDoor* one = m_target ? m_target : m_pObjTarget;  // 1:1
		if (one) one->OpenDoor();
	}

	if (m_linkMode == Engine::LINKMODE::NONE) {			// 스테이지 끝(조건을 m_linkId -> m_linkMode로 변경 (08/31 김홍인))
		if (auto* st = dynamic_cast<CStage*>(CManagement::GetInstance()->Get_Scene())) {
			st->EnterResultMode();
		}
		else if (auto* st2 = dynamic_cast<CStage2*>(CManagement::GetInstance()->Get_Scene())) {
			st2->EnterResultMode();
		}
		else if (auto* st3 = dynamic_cast<CStage3*>(CManagement::GetInstance()->Get_Scene())) {
			st3->EnterResultMode();
		}
	}
}

CSwitch_Button* CSwitch_Button::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSwitch_Button* pSwitch = new CSwitch_Button(pGraphicDev);

	return pSwitch;
}

CSwitch_Button* CSwitch_Button::Clone()
{
	CSwitch_Button* pSB = new CSwitch_Button(*this);

	if (FAILED(pSB->Ready_GameObject()))
	{
		Safe_Release(pSB);
		MSG_BOX("Switch Create Failed");
		return nullptr;
	}

	return pSB;
}

void CSwitch_Button::Free()
{
	m_state == State::Idle;
	m_bPressed = false;
	Engine::CGameObject::Free();
}
