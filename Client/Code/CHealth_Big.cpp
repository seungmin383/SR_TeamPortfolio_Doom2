#include "pch.h"
#include "CHealth_Big.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CStatsManager.h"
#include "CTerrainSector.h"

CHealth_Big::CHealth_Big(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_iHeal(0)
{
}

CHealth_Big::CHealth_Big(const CGameObject& rhs)
    :CGameObject(rhs)
{
}

CHealth_Big::~CHealth_Big()
{
}

HRESULT CHealth_Big::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_iHeal = 100;

	m_pTransformCom->Set_Pos(3.f, 0.f, 7.f);
	m_pTransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
		_vec3(0, 0.4f, 0),        // 약간 위로
		COL_ITEM,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYER       // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	return S_OK;
}

_int CHealth_Big::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Set_OnTerrain();
	Bill_Board();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_bPicked)
		return -1; // 엔진 상수에 맞춰 사용

	return 0;
}

void CHealth_Big::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // ★ 중요
}

void CHealth_Big::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CHealth_Big::OnCollision(CCollider* _pOther)
{
}

void CHealth_Big::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[HB] OnCollisionEnter\n");

	if (m_bPicked || !_pOther) return;

	auto* who = _pOther->Get_Owner();
	if (auto* pCam = dynamic_cast<CDynamicCamera*>(who)) {
		// 1) HP 증가
		pCam->AddHP(m_iHeal); // CDynamicCamera에 AddHP(int) 추가

		// 2) 더이상 충돌하지 않게 막기
		m_bPicked = true;

		CStatsManager::I().OnItemPicked();
	}
}

void CHealth_Big::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CHealth_Big::Add_Component()
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
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Heal_Big"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Health_Big\n");

	return S_OK;
}

void CHealth_Big::Set_OnTerrain()
{
	_vec3 pos; m_pTransformCom->Get_Pos(&pos);

	_float highest(-999.f);
	_vec3 onPosition{ 0.f, 0.f, 0.f };
	_vec3 m_vEye{ 65.f, 0, 5.f };
	_vec3 m_vAt{ 0.f, 0.f, 1.f };
	for (auto terrain : CManagement::GetInstance()->Get_Scene()->Get_Terrain())
	{
		auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();

		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &pos))
		{
			if (highest < onPosition.y)
				highest = onPosition.y;
		}
	}

	if (highest == -999.f)
		highest = 0.f;

	_float deltaHeight = (highest + 4.f) - m_vEye.y;
	m_vEye.y += deltaHeight;
	m_vAt.y += deltaHeight;
}

void CHealth_Big::Bill_Board()
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

CHealth_Big* CHealth_Big::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHealth_Big* pHealthBig = new CHealth_Big(pGraphicDev);

	return pHealthBig;
}

CHealth_Big* CHealth_Big::Clone()
{
	CHealth_Big* pHBig = new CHealth_Big(*this);

	if (FAILED(pHBig->Ready_GameObject()))
	{
		Safe_Release(pHBig);
		MSG_BOX("Health Big Create Failed");
		return nullptr;
	}

	return pHBig;
}

void CHealth_Big::Free()
{
	Engine::CGameObject::Free();
}
