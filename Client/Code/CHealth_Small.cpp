#include "pch.h"
#include "CHealth_Small.h"
#include "CRenderer.h"
#include "CManagement.h"

CHealth_Small::CHealth_Small(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_iHeal(0), m_iFrame(0)
{
}

CHealth_Small::CHealth_Small(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

CHealth_Small::~CHealth_Small()
{
}

HRESULT CHealth_Small::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_iHeal = 1;

	m_iFrame = 0;

	D3DXMATRIX view, inv;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);
	D3DXMatrixInverse(&inv, nullptr, &view);

	// 카메라 월드 위치/정면벡터
	_vec3 camPos = { inv._41, inv._42, inv._43 };
	_vec3 camLook = { inv._31, inv._32, inv._33 }; // LH 기준 Look

	// 카메라 앞 2.5m + 살짝 위
	_vec3 spawn = { camPos.x + camLook.x * 2.5f,
					camPos.y + camLook.y * 2.5f + 1.f,
					camPos.z + camLook.z * 2.5f };

	m_pTransformCom->Set_Pos(spawn.x, spawn.y, spawn.z);
	m_pTransformCom->Set_Scale(_vec3(0.4f, 0.4f, 1.f)); // 0.3~0.6 사이에서 취향대로

    return S_OK;
}

_int CHealth_Small::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	Set_OnTerrain();
	// 월드 최신화 보장 (Transform은 가벼움)
	m_pTransformCom->Update_Component(0.f);
	//Bill_Board();

	m_iFrame++;
	if (m_iFrame >= 4)
		m_iFrame = 0;

    return 0;
}

void CHealth_Small::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CHealth_Small::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Set_Texture(m_iFrame);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pBufferCom->Render_Buffer();

	// 되돌림
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	//충돌체
	//m_pColliderCom->Render(g_colRend);
}

void CHealth_Small::OnCollision(CCollider* _pOther)
{
}

void CHealth_Small::OnCollisionEnter(CCollider* _pOther)
{
}

void CHealth_Small::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CHealth_Small::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectTex*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_RectTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Heal_Small"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CHealth_Small::Set_OnTerrain()
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
	m_pTransformCom->Set_Pos(pos.x, h + 0.8f, pos.z);
}

void CHealth_Small::Bill_Board()
{
	_matrix world, view, rotOnly;
	m_pTransformCom->Get_World(&world);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &view);

	// 1) 카메라에서 회전 성분만 뽑아 역행렬로 빌보드 회전 만들기
	D3DXMatrixIdentity(&rotOnly);
	rotOnly._11 = view._11; rotOnly._13 = view._13;
	rotOnly._31 = view._31; rotOnly._33 = view._33;
	D3DXMatrixInverse(&rotOnly, 0, &rotOnly); // 카메라 회전의 역행렬

	// 2) 기존 월드의 위치/스케일 보존
	_vec3 pos = { world._41, world._42, world._43 };
	world = rotOnly;                  // 회전만 교체
	world._41 = pos.x;                // 위치 되살리기
	world._42 = pos.y;
	world._43 = pos.z;

	m_pTransformCom->Set_World(&world);
}

CHealth_Small* CHealth_Small::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHealth_Small* pHealthSmall = new CHealth_Small(pGraphicDev);

	if (FAILED(pHealthSmall->Ready_GameObject()))
	{
		Safe_Release(pHealthSmall);
		MSG_BOX("Health Small Create Failed");
		return nullptr;
	}

	return pHealthSmall;
}

void CHealth_Small::Free()
{
    Engine::CGameObject::Free();
}
