#include "pch.h"
#include "CWeapon_Shotgun.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CWeaponManager.h"
#include "CGameUI_Face.h"
#include "FaceUIRegistry.h"
#include "CStatsManager.h"
#include "CShotgun.h"
#include "CTerrainSector.h"

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
	m_pTransformCom->Set_Scale(_vec3(1.f, 0.5f, 1.f));

	m_pWeaponManager = CWeaponManager::GetInstance();

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
		_vec3(0, 0.4f, 0),        // 약간 위로
		COL_WEAPON,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYER     // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	return S_OK;
}

_int CWeapon_Shotgun::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Set_OnTerrain();
	Bill_Board();

	// 픽업 후에는 렌더 그룹 등록 금지
	if (!m_bPicked) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	}
	else {
		// 엔진에서 -1 리턴으로 객체 제거를 처리한다면 그대로 두고,
		// 아래 OnCollisionEnter에서 Set_Dead()도 같이 호출해 안전하게 제거
		return -1;
	}

	return 0;
}

void CWeapon_Shotgun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom && !m_bPicked)
		m_pColliderCom->Late_Update();
}

void CWeapon_Shotgun::Render_GameObject(const _float& fTimeDelta)
{
	if (m_bPicked)
		return;

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

	// 상대 레이어가 플레이어인지 확인
	const auto layer = _pOther->Get_ColLayer();
	if ((layer & (COL_PLAYER)) == 0)
		return;

	// 플레이어 판정 제대로: Collider의 Owner 기준
	auto* owner = _pOther->Get_Owner();
	if (!owner) return;
	// 네 프로젝트에서 실제 플레이어 타입으로 바꾸기: CDynamicCamera 또는 CPlayer
	bool isPlayer = (dynamic_cast<CDynamicCamera*>(owner) != nullptr);
	if (!isPlayer) return;

	m_bPicked = true;

	// 얼굴 애니메이션
	if (Client::g_FaceUI) {
		Client::g_FaceUI->Set_EvilFace(3.f);
	}
	else {
		OutputDebugStringA("[WS] g_FaceUI is null. CStage에서 등록됐는지 확인\n");
	}

	CWeaponManager* pWeaponManager = CWeaponManager::GetInstance();
	if (pWeaponManager->Get_Weapon(WEAPON_ID::SHOTGUN) == nullptr)
	{
		CWeapon* pShotgun = CShotgun::Create();
		pWeaponManager->Add_Weapon(WEAPON_ID::SHOTGUN, pShotgun);
		pWeaponManager->Change_Weapon(WEAPON_ID::SHOTGUN);
		pWeaponManager->Add_Ammo(AMMO_TYPE::SHELL, 8);
	}
	else
		pWeaponManager->Add_Ammo(AMMO_TYPE::SHELL, 8);


	if (m_pColliderCom)
	{
		m_pColliderCom->Set_ColLayer((COLLAYERID)0);			// 충돌 레이어를 NONE으로
		m_pColliderCom->Set_Active(false);
	}
	// Update_GameObject()에서 m_bPicked면 -1 반환 → 레이어에서 제거

	Set_Dead(true);
	
	CStatsManager::I().OnItemPicked();
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
	_float highest(FLT_MIN);
	_vec3 pos;
	_vec3 onPosition{ 0.f, 0.f, 0.f };
	_float vScaleY = m_pTransformCom->Get_Scale()->y;
	for (auto terrain : CManagement::GetInstance()->Get_Scene()->Get_Terrain())
	{
		m_pTransformCom->Get_Pos(&pos);

		auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();


		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &pos))
		{
			if (highest < onPosition.y)
				highest = onPosition.y;
		}
	}

	m_pTransformCom->Set_Pos(pos.x, highest + vScaleY, pos.z);
}

void CWeapon_Shotgun::Bill_Board()
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
