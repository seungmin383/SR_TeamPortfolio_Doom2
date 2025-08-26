#include "pch.h"
#include "CWeapon_BFG.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CWeaponManager.h"
#include "CGameUI_Face.h"
#include "FaceUIRegistry.h"
#include "CStatsManager.h"
#include "CBfg.h"
#include "CTerrainSector.h"

CWeapon_BFG::CWeapon_BFG(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CWeapon_BFG::CWeapon_BFG(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CWeapon_BFG::~CWeapon_BFG()
{
}

HRESULT CWeapon_BFG::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_bPicked = false;

	m_pTransformCom->Set_Pos(50.f, 0.f, 40.f);
	m_pTransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

	m_pWeaponManager = CWeaponManager::GetInstance();

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
		_vec3(0, 0.4f, 0),        // 약간 위로
		COL_WEAPON,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYER       // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	return S_OK;
}

_int CWeapon_BFG::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Set_OnTerrain();
	Bill_Board();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_bPicked)
		return -1; // 엔진 상수에 맞춰 사용

	return 0;
}

void CWeapon_BFG::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update();
}

void CWeapon_BFG::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CWeapon_BFG::OnCollision(CCollider* _pOther)
{
}

void CWeapon_BFG::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[BFG] OnCollisionEnter\n");
	if (m_bPicked || !_pOther) return;

	const auto layer = _pOther->Get_ColLayer();
	if ((layer & (COL_PLAYER)) == 0)
		return;

	// 플레이어 판정
	_bool isPlayer = true;
	if (auto* owner = _pOther->Get_Owner()) {

	}
	if (!isPlayer) return;

	m_bPicked = true;

	// 얼굴 애니메이션
	if (Client::g_FaceUI) {
		Client::g_FaceUI->Set_EvilFace(3.f);
	}
	else {
		OutputDebugStringA("[BFG] g_FaceUI is null. CStage에서 등록됐는지 확인\n");
	}

	CWeaponManager* pWeaponManager = CWeaponManager::GetInstance();
	if (pWeaponManager->Get_Weapon(WEAPON_ID::BFG) == nullptr)
	{
		CWeapon* pBfg = CBfg::Create();
		pWeaponManager->Add_Weapon(WEAPON_ID::BFG, pBfg);
		pWeaponManager->Change_Weapon(WEAPON_ID::BFG);
		pWeaponManager->Add_Ammo(AMMO_TYPE::CELL, 5);
	}
	else
		pWeaponManager->Add_Ammo(AMMO_TYPE::CELL, 5);


	if (m_pColliderCom) m_pColliderCom->Set_Active(false);
	// Update_GameObject()에서 m_bPicked이면 -1 반환 → 레이어가 안전 삭제

	CStatsManager::I().OnItemPicked();
}

void CWeapon_BFG::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CWeapon_BFG::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_BFG"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: BFG\n");

	return S_OK;
}

void CWeapon_BFG::Set_OnTerrain()
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

void CWeapon_BFG::Bill_Board()
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

CWeapon_BFG* CWeapon_BFG::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWeapon_BFG* pBFG = new CWeapon_BFG(pGraphicDev);

	return pBFG;
}

CWeapon_BFG* CWeapon_BFG::Clone()
{
	CWeapon_BFG* pBFG = new CWeapon_BFG(*this);

	if (FAILED(pBFG->Ready_GameObject()))
	{
		Safe_Release(pBFG);
		MSG_BOX("BFG Create Failed");
		return nullptr;
	}

	return pBFG;
}

void CWeapon_BFG::Free()
{
	Engine::CGameObject::Free();
}
