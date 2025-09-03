#include "pch.h"
#include "CAmmo_Clip.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CWeaponManager.h"
#include "CStatsManager.h"
#include "CTerrainSector.h"
#include "CSoundManager.h"

CAmmo_Clip::CAmmo_Clip(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_iClip(0), m_iFrame_Clip(0), m_pWeaponManager(nullptr)
{
}

CAmmo_Clip::CAmmo_Clip(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CAmmo_Clip::~CAmmo_Clip()
{
}

HRESULT CAmmo_Clip::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_iClip = 10;

	m_iFrame_Clip = 0;

	m_pWeaponManager = CWeaponManager::GetInstance();

	m_pTransformCom->Set_Pos(8.f, 0.f, 10.f);
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

	return S_OK;
}

_int CAmmo_Clip::Update_GameObject(const _float& fTimeDelta)
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

void CAmmo_Clip::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom && !m_bPicked)
		m_pColliderCom->Late_Update(); // 중요
}

void CAmmo_Clip::Render_GameObject(const _float& fTimeDelta)
{
	if (m_bPicked)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iFrame_Clip);
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CAmmo_Clip::OnCollision(CCollider* _pOther)
{
}

void CAmmo_Clip::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[Clip] OnCollisionEnter\n");

	if (m_bPicked || !_pOther) return;

	if ((_pOther->Get_ColLayer() & COL_PLAYER) == 0) return;
	auto* owner = _pOther->Get_Owner();
	if (!owner || !dynamic_cast<CDynamicCamera*>(owner)) return; // 실제 플레이어 타입으로

	m_bPicked = true;

	if (m_pColliderCom) {
		m_pColliderCom->Set_ColLayer((COLLAYERID)0);
		m_pColliderCom->Set_Active(false);
	}

	CWeaponManager::GetInstance()->Add_Ammo(AMMO_TYPE::CLIP, 10);

	Set_Dead(true);

	CStatsManager::I().OnItemPicked();
	CSoundManager::GetInstance()->PlaySoundW(L"DSITEMUP.wav", CHANNELID::SOUND_EFFECT, 0.8f);
}

void CAmmo_Clip::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CAmmo_Clip::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Clip"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Ammo_Clip\n");

	return S_OK;
}

void CAmmo_Clip::Set_OnTerrain()
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

		_vec3 onPosition{ 0.f, 0.f, 0.f };
		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &pos))
		{

			m_pTransformCom->Set_Pos(pos.x, onPosition.y + 2, pos.z);
			break;
		}
	}
}

void CAmmo_Clip::Bill_Board()
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

CAmmo_Clip* CAmmo_Clip::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAmmo_Clip* pAmmoClip = new CAmmo_Clip(pGraphicDev);

	return pAmmoClip;
}

CAmmo_Clip* CAmmo_Clip::Clone()
{
	CAmmo_Clip* pAC = new CAmmo_Clip(*this);

	if (FAILED(pAC->Ready_GameObject()))
	{
		Safe_Release(pAC);
		MSG_BOX("Ammo Clip Create Failed");
		return nullptr;
	}

	return pAC;
}

void CAmmo_Clip::Free()
{
	Engine::CGameObject::Free();
}
