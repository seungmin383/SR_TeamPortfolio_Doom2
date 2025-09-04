#include "pch.h"
#include "CAmmo_Cell.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CWeaponManager.h"
#include "CStatsManager.h"
#include "CTerrainSector.h"
#include "CSoundManager.h"

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
	m_pTransformCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
		_vec3(0, 0.4f, 0),        // 약간 위로
		COL_ITEM,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYER       // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	Set_OnTerrain();

	return S_OK;
}

_int CAmmo_Cell::Update_GameObject(const _float& fTimeDelta)
{
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	LONG minx = min(pos.x + 5, pos.x - 5);
	LONG maxx = max(pos.x + 5, pos.x - 5);
	LONG minz = min(pos.z + 5, pos.z - 5);
	LONG maxz = max(pos.z + 5, pos.z - 5);

	RECT myRect{ minx, minz, maxx, maxz };
	if (CCollisionManager::Check_AABB(myRect) == false)
		return 0;

	CGameObject::Update_GameObject(fTimeDelta);
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

	m_pTextureCom->Set_Texture(m_iFrame_Cell);
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
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
		CSoundManager::GetInstance()->PlaySoundW(L"DSITEMUP.wav", CHANNELID::SOUND_EFFECT, 0.8f);
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
	_float highest(-999.f);
	_vec3 pos;
	_vec3 onPosition{ 0.f, 0.f, 0.f };
	_float vScaleY = m_pTransformCom->Get_Scale()->y;

	for (auto terrain : CManagement::GetInstance()->Get_Scene()->Get_Terrain())
	{
		m_pTransformCom->Get_Pos(&pos);

		auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();

		_vec3 onPosition{ 0.f, 0.f, 0.f };
		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &pos))
		{
			if (highest < onPosition.y)
				highest = onPosition.y;
		}
	}

	if (highest == -999.f)
		highest = 0.f;

	m_pTransformCom->Set_Pos(pos.x, highest + 2, pos.z);
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
