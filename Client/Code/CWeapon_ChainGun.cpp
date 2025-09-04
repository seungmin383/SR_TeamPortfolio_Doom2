#include "pch.h"
#include "CWeapon_ChainGun.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CWeaponManager.h"
#include "CGameUI_Face.h"
#include "FaceUIRegistry.h"
#include "CStatsManager.h"
#include "CChaingun.h"
#include "CTerrainSector.h"
#include "CSoundManager.h"

CWeapon_ChainGun::CWeapon_ChainGun(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CWeapon_ChainGun::CWeapon_ChainGun(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CWeapon_ChainGun::~CWeapon_ChainGun()
{
}

HRESULT CWeapon_ChainGun::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_bPicked = false;

	m_pTransformCom->Set_Pos(35.f, 0.f, 10.f);
	m_pTransformCom->Set_Scale(_vec3(1.f, 0.5f, 1.f));

	m_pWeaponManager = CWeaponManager::GetInstance();

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
		_vec3(0, 0.4f, 0),        // 약간 위로
		COL_WEAPON,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYER      // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	Set_OnTerrain();

	return S_OK;
}

_int CWeapon_ChainGun::Update_GameObject(const _float& fTimeDelta)
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

	if (CManagement::GetInstance()->Is_Paused())
		return 0;

	CGameObject::Update_GameObject(fTimeDelta);
	Bill_Board();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_bPicked)
		return -1; // 엔진 상수에 맞춰 사용

	if (!m_bPicked)
		return Calc_Death_Time(fTimeDelta);

	return 0;
}

void CWeapon_ChainGun::LateUpdate_GameObject(const _float& fTimeDelta)
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

	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update();
}

void CWeapon_ChainGun::Render_GameObject(const _float& fTimeDelta)
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

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
}

void CWeapon_ChainGun::OnCollision(CCollider* _pOther)
{
}

void CWeapon_ChainGun::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[WCG] OnCollisionEnter\n");
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
		OutputDebugStringA("[WCG] g_FaceUI is null. CStage에서 등록됐는지 확인\n");
	}

	CWeaponManager* pWeaponManager = CWeaponManager::GetInstance();
	if (pWeaponManager->Get_Weapon(WEAPON_ID::CHAINGUN) == nullptr)
	{
		CWeapon* pChaingun = CChaingun::Create();
		pWeaponManager->Add_Weapon(WEAPON_ID::CHAINGUN, pChaingun);
		pWeaponManager->Change_Weapon(WEAPON_ID::CHAINGUN);
		pWeaponManager->Add_Ammo(AMMO_TYPE::CLIP, 50);
	}
	else
		pWeaponManager->Add_Ammo(AMMO_TYPE::CLIP, 50);

	// 월드의 픽업 오브젝트는 제거
	
	if (m_pColliderCom) m_pColliderCom->Set_Active(false);
	// Update_GameObject()에서 m_bPicked이면 -1 반환 → 레이어가 안전 삭제

	CStatsManager::I().OnItemPicked();
	CSoundManager::GetInstance()->PlaySoundW(L"DSWPNUP.wav", CHANNELID::SOUND_EFFECT, 0.8f);
}

void CWeapon_ChainGun::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CWeapon_ChainGun::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_ChainGun"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: ChainGun\n");

	return S_OK;
}

void CWeapon_ChainGun::Set_OnTerrain()
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

void CWeapon_ChainGun::Bill_Board()
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

_int CWeapon_ChainGun::Calc_Death_Time(const _float fTimeDelta)
{
	m_fDeathTimer += fTimeDelta;

	if (m_fDeathTimer > 3.5f)
	{
		m_fDeathTimer = 0.f;
		return -1;
	}
	return 0;
}

CWeapon_ChainGun* CWeapon_ChainGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWeapon_ChainGun* pChaingun = new CWeapon_ChainGun(pGraphicDev);

	return pChaingun;
}

CWeapon_ChainGun* CWeapon_ChainGun::Clone()
{
	CWeapon_ChainGun* pWCG = new CWeapon_ChainGun(*this);

	if (FAILED(pWCG->Ready_GameObject()))
	{
		Safe_Release(pWCG);
		MSG_BOX("Chain Gun Create Failed");
		return nullptr;
	}

	return pWCG;
}

void CWeapon_ChainGun::Free()
{
	Engine::CGameObject::Free();
}
