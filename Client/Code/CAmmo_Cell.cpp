#include "pch.h"
#include "CAmmo_Cell.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CWeaponManager.h"
#include "CStatsManager.h"
#include "CTerrainSector.h"

CAmmo_Cell::CAmmo_Cell(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_iCell(0), m_iFrame_Cell(0), m_pWeaponManager(nullptr)
{
}

CAmmo_Cell::CAmmo_Cell(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

CAmmo_Cell::~CAmmo_Cell()
{	
}

HRESULT CAmmo_Cell::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_iCell = 3;

	m_iFrame_Cell = 0;

	m_pWeaponManager = CWeaponManager::GetInstance();

	m_pTransformCom->Set_Pos(13.f, 0.f, 11.f);
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

_int CAmmo_Cell::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Set_OnTerrain();
	Bill_Board();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_bPicked)
		return -1; // 엔진 상수에 맞춰 사용

	return 0;
}

void CAmmo_Cell::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // 중요
}

void CAmmo_Cell::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iFrame_Cell);
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CAmmo_Cell::OnCollision(CCollider* _pOther)
{
}

void CAmmo_Cell::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[Cell] OnCollisionEnter\n");

	if (m_bPicked || !_pOther) return;

	// 누가 부딪혔는지: 플레이어/카메라
	const auto layer = _pOther->Get_ColLayer();
	const bool allowed =
		(layer & COL_PLAYER);

	if (!allowed) return;
	else {
		// 무기 매니저로 탄약 지급
		if (m_pWeaponManager) {
			m_pWeaponManager->Add_Ammo(AMMO_TYPE::CELL, m_iCell);
		}

		// 아이템 비활성화(충돌/업데이트에서 제거)
		m_bPicked = true;
		if (m_pColliderCom) m_pColliderCom->Set_Active(false);

		CStatsManager::I().OnItemPicked();
	}
}

void CAmmo_Cell::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CAmmo_Cell::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Cell"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Ammo_Cell\n");

	return S_OK;
}

void CAmmo_Cell::Set_OnTerrain()
{
	//if (!m_pTransformCom || !m_pCalculatorCom) return;

	//auto* pTerrainBuf = static_cast<CTerrainTexSector*>(
	//	CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));
	//if (!pTerrainBuf) return;

	//_vec3 pos; m_pTransformCom->Get_Pos(&pos);

	//// 그리드 경계(0..(VTXCNT-1)*VTXITV) 밖이면 보정 건너뜀 (혹은 clamp)
	//const float maxX = (VTXCNTX - 1) * VTXITV - 1e-3f;
	//const float maxZ = (VTXCNTZ - 1) * VTXITV - 1e-3f;
	//if (pos.x < 0.f || pos.z < 0.f || pos.x > maxX || pos.z > maxZ) return;

	//float h = m_pCalculatorCom->Compute_HeightOnTerrain(&pos, pTerrainBuf->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	//// 살짝 띄워서 지면 간섭 방지
	//m_pTransformCom->Set_Pos(pos.x, h + 1.f, pos.z);

	_float highest(-999.f);
	_vec3 onPosition{ 0.f, 0.f, 0.f };
	_vec3 m_vEye{ 65.f, 0, 5.f };
	_vec3 m_vAt{ 0.f, 0.f, 1.f };
	for (auto terrain : CManagement::GetInstance()->Get_Scene()->Get_Terrain())
	{
		auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();

		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition))
		{
			if (highest < onPosition.y)
				highest = onPosition.y;
		}
	}

	if (highest == -999.f)
		highest = 0.f;

	_float deltaHeight = (highest + 3.f) - m_vEye.y;
	m_vEye.y += deltaHeight;
	m_vAt.y += deltaHeight;
}

void CAmmo_Cell::Bill_Board()
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

CAmmo_Cell* CAmmo_Cell::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAmmo_Cell* pAmmoCell = new CAmmo_Cell(pGraphicDev);

	return pAmmoCell;
}

CAmmo_Cell* CAmmo_Cell::Clone()
{
	CAmmo_Cell* pAC = new CAmmo_Cell(*this);

	if (FAILED(pAC->Ready_GameObject()))
	{
		Safe_Release(pAC);
		MSG_BOX("Ammo Clip Create Failed");
		return nullptr;
	}

	return pAC;
}

void CAmmo_Cell::Free()
{
	Engine::CGameObject::Free();
}
