#include "pch.h"
#include "CArmor_Big.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CStatsManager.h"
#include "CTerrainSector.h"
#include "CSoundManager.h"

CArmor_Big::CArmor_Big(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_iArmor(0), m_iFrame_Armor(0)
{
}

CArmor_Big::CArmor_Big(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CArmor_Big::~CArmor_Big()
{
}

HRESULT CArmor_Big::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_iArmor = 200;

	m_iFrame_Armor = 0;

	m_pTransformCom->Set_Pos(15.f, 0.f, 15.f);
	m_pTransformCom->Set_Scale(_vec3(0.9f, 1.f, 0.9f));

	m_pColliderCom->Ready_CCollider(
		this,
		_vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
		_vec3(1.1f, 1.1f, 1.1f),  // 아이템 박스 크기 예시
		_vec3(0.f, 0.f, 0.f),        // 약간 위로
		COL_ITEM,               // 너희 COLLAYERID에서 아이템 레이어
		COL_PLAYER       // (마스크를 쓴다면) 플레이어와만 맞게
	);
	m_pColliderCom->Set_Active(true);

	Set_OnTerrain();

	return S_OK;
}
_int CArmor_Big::Update_GameObject(const _float& fTimeDelta)
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

	m_iFrame_Armor++;
	if (m_iFrame_Armor >= 2)
		m_iFrame_Armor = 0;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_bPicked)
		return -1; // 엔진 상수에 맞춰 사용

	return 0;
}

void CArmor_Big::LateUpdate_GameObject(const _float& fTimeDelta)
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
		m_pColliderCom->Late_Update(); // ★ 중요
}

void CArmor_Big::Render_GameObject(const _float& fTimeDelta)
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

	if (CManagement::GetInstance()->Is_Paused())
		m_pTextureCom->Set_Texture(0);
	else
		m_pTextureCom->Set_Texture(m_iFrame_Armor);
	
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
}

void CArmor_Big::OnCollision(CCollider* _pOther)
{
}

void CArmor_Big::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[AB] OnCollisionEnter\n");

	if (m_bPicked || !_pOther) return;

	auto* who = _pOther->Get_Owner();
	if (auto* pCam = dynamic_cast<CDynamicCamera*>(who)) {
		// 1) HP 증가
		pCam->AddArmor(m_iArmor); // CDynamicCamera에 AddHP(int) 추가

		// 2) 더이상 충돌하지 않게 막기
		m_bPicked = true;

		CStatsManager::I().OnItemPicked();
		CSoundManager::GetInstance()->PlaySoundW(L"DSITEMUP.wav", CHANNELID::SOUND_EFFECT, 0.8f);
	}
}

void CArmor_Big::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CArmor_Big::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Armor_Big"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Armor_Big\n");

	return S_OK;
}

void CArmor_Big::Set_OnTerrain()
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

void CArmor_Big::Bill_Board()
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

CArmor_Big* CArmor_Big::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CArmor_Big* pArmorBig = new CArmor_Big(pGraphicDev);

	return pArmorBig;
}

CArmor_Big* CArmor_Big::Clone()
{
	CArmor_Big* pABig = new CArmor_Big(*this);

	if (FAILED(pABig->Ready_GameObject()))
	{
		Safe_Release(pABig);
		MSG_BOX("Armor Small Create Failed");
		return nullptr;
	}

	return pABig;
}

void CArmor_Big::Free()
{
	Engine::CGameObject::Free();
}
