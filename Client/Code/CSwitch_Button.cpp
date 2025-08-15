#include "pch.h"
#include "CSwitch_Button.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"

CSwitch_Button::CSwitch_Button(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
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

	m_pTransformCom->Set_Pos(15.f, 0.f, 18.f);
	m_pTransformCom->Set_Scale(_vec3(0.1f, 0.1f, 0.1f));

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
		_vec3(0, 0.4f, 0),        // 약간 위로
		COL_ITEM,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYERCAMERA       // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	return S_OK;
}

_int CSwitch_Button::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Set_OnTerrain();
	Bill_Board();

	//SetSwitchOn();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CSwitch_Button::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // ★ 중요
}

void CSwitch_Button::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (m_bPressed)
		m_pTextureCom2->Set_Texture();
	else
	{
		m_pTextureCom->Set_Texture();
		//SetSwitchOn();// 문 열림 처리
	}


	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CSwitch_Button::OnCollision(CCollider* _pOther)
{
}

void CSwitch_Button::OnCollisionEnter(CCollider* _pOther)
{
	if (!_pOther) return;

	if (m_state == State::Idle)
	{
		const auto layer = _pOther->Get_ColLayer();

		// 플레이어랑만 처리
		if (layer & (COL_PLAYERCAMERA)) {
			m_bPressed = true;
			m_state = State::Dead;
		}
	}
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
	m_pTransformCom->Set_Pos(pos.x, h + 0.7f, pos.z);
}

void CSwitch_Button::Bill_Board()
{
	if (!m_pTransformCom)
		return;

	_matrix matWorld, matView, matBill;
	m_pTransformCom->Get_World(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	// 빌보드 행렬의 스케일과 위치는 월드 행렬의 것을 그대로 사용                          
	matBill = matWorld;

	// 뷰행렬에서 역행렬을 이용해 카메라의 회전값을 구하고, 빌보드 행렬의 회전값으로 설정  
	D3DXMatrixInverse(&matBill, NULL, &matView);

	// 빌보드 행렬의 위치값을 월드 행렬의 위치값으로 설정                                  
	matBill._41 = matWorld._41;
	matBill._42 = matWorld._42;
	matBill._43 = matWorld._43;

	// 최종적으로 빌보드 행렬을 월드 행렬로 설정                                           
	m_pTransformCom->Set_World(&matBill);
}

void CSwitch_Button::SetSwitchOn()
{

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
	Engine::CGameObject::Free();
}
