#include "pch.h"
#include "CSuits.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CWeaponManager.h"
#include "CStatsManager.h"
#include "CTerrainSector.h"
#include "FaceUIRegistry.h"

CSuits::CSuits(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CSuits::CSuits(const CGameObject& rhs)
	:CGameObject(rhs)
{
}

CSuits::~CSuits()
{
}

HRESULT CSuits::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(25.f, 0.f, 11.f);					// 임시로 추가
	m_pTransformCom->Set_Scale(_vec3(0.9f, 1.f, 0.9f));

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

_int CSuits::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Bill_Board();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_bPicked)
		return -1; // 엔진 상수에 맞춰 사용

	return 0;
}

void CSuits::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // 중요
}

void CSuits::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CSuits::OnCollision(CCollider* _pOther)
{
}

void CSuits::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[Suit] OnCollisionEnter\n");

	if (m_bPicked || !_pOther) return;

	// 상대 레이어가 플레이어인지 확인
	const auto layer = _pOther->Get_ColLayer();
	if ((layer & (COL_PLAYER)) == 0)
		return;

	// 플레이어 판정 제대로: Collider의 Owner 기준
	auto* owner = _pOther->Get_Owner();
	if (!owner) return;

	bool isPlayer = (dynamic_cast<CDynamicCamera*>(owner) != nullptr);
	if (!isPlayer) return;

	m_bPicked = true;

	// 얼굴 애니메이션
	//if (Client::g_FaceUI) {
	//	Client::g_FaceUI->Set_EvilFace(3.f);
	//}

	//else {
	//	OutputDebugStringA("[WS] g_FaceUI is null. CStage에서 등록됐는지 확인\n");
	//}

	if (m_pColliderCom)
	{
		m_pColliderCom->Set_ColLayer((COLLAYERID)0);
		m_pColliderCom->Set_Active(false);
	}
	CStatsManager::I().OnItemPicked();						// 아이템 획득을 알려주는 코드
}

void CSuits::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CSuits::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Suit"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Suit\n");

	return S_OK;
}

void CSuits::Set_OnTerrain()
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
			if (highest < onPosition.y)
				highest = onPosition.y;
		}
	}

	m_pTransformCom->Set_Pos(pos.x, highest + 2, pos.z);
}

void CSuits::Bill_Board()
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

CSuits* CSuits::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSuits* pSuit = new CSuits(pGraphicDev);

	return pSuit;
}

CSuits* CSuits::Clone()
{
	CSuits* pSuit = new CSuits(*this);

	if (FAILED(pSuit->Ready_GameObject()))
	{
		Safe_Release(pSuit);
		MSG_BOX("Suit Create Failed");
		return nullptr;
	}

	return pSuit;
}

void CSuits::Free()
{
	Engine::CGameObject::Free();
}

