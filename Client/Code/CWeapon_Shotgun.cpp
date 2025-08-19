#include "pch.h"
#include "CWeapon_Shotgun.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"

CWeapon_Shotgun::CWeapon_Shotgun(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CWeapon_Shotgun::CWeapon_Shotgun(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CWeapon_Shotgun::~CWeapon_Shotgun()
{
}

HRESULT CWeapon_Shotgun::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_bPicked = false;

	m_pTransformCom->Set_Pos(22.f, 0.f, 10.f);
	m_pTransformCom->Set_Scale(_vec3(1.f, 0.1f, 0.1f));

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
		_vec3(0, 0.4f, 0),        // 약간 위로
		COL_ITEM,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYER | COL_PLAYERCAMERA       // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	return S_OK;
}

_int CWeapon_Shotgun::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Set_OnTerrain();
	Bill_Board();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_bPicked)
		return -1; // 엔진 상수에 맞춰 사용

	return 0;
}

void CWeapon_Shotgun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update();
}

void CWeapon_Shotgun::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CWeapon_Shotgun::OnCollision(CCollider* _pOther)
{
}

void CWeapon_Shotgun::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[WS] OnCollisionEnter\n");
	if (m_bPicked || !_pOther) return;

	const auto layer = _pOther->Get_ColLayer();
	if (!(layer & (COL_PLAYER | COL_PLAYERCAMERA)))
		return;

	// to-do) 무기매니저로 캐스팅해서 얻기
	if (auto* mgr = CWeaponManager::GetInstance()) {
		if (CWeapon* inv = (CWeapon*)CWeapon_Shotgun::Create(m_pGraphicDev)) { // 이 코드 상태에서 누수 발생
			mgr->Add_Weapon(CWeaponManager::WEAPON_ID::SHOTGUN, inv); // 이 코드 상태에서 누수 발생
			
			// mgr->Change_Weapon(CWeaponManager::WEAPON_ID::SHOTGUN);
		}
	}
	// 월드의 픽업 오브젝트는 제거
	m_bPicked = true;
	if (m_pColliderCom) m_pColliderCom->Set_Active(false);
	// Update_GameObject()에서 m_bPicked이면 -1 반환 → 레이어가 안전 삭제
}

void CWeapon_Shotgun::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CWeapon_Shotgun::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Shotgun"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Shotgun\n");

	return S_OK;
}

void CWeapon_Shotgun::Set_OnTerrain()
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

void CWeapon_Shotgun::Bill_Board()
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

CWeapon_Shotgun* CWeapon_Shotgun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWeapon_Shotgun* pShotgun = new CWeapon_Shotgun(pGraphicDev);

	return pShotgun;
}

CWeapon_Shotgun* CWeapon_Shotgun::Clone()
{
	CWeapon_Shotgun* pWS = new CWeapon_Shotgun(*this);

	if (FAILED(pWS->Ready_GameObject()))
	{
		Safe_Release(pWS);
		MSG_BOX("Shotgun Create Failed");
		return nullptr;
	}

	return pWS;
}

void CWeapon_Shotgun::Free()
{
	Engine::CGameObject::Free();
}
