#include "pch.h"
#include "CKey.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CStatsManager.h"
#include "CTerrainSector.h"
#include "CSoundManager.h"
#include"CObjectFactory.h"
#include "CFileManager.h"

CKey::CKey(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CKey::CKey(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CKey::~CKey()
{
}

HRESULT CKey::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(40.f, 0.f, 40.f);
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

_int CKey::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Bill_Board();

	m_iFrame_Key++;
	if (m_iFrame_Key >= 2)
		m_iFrame_Key = 0;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_bPicked)
		return -1; // 엔진 상수에 맞춰 사용

	return 0;
}

void CKey::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // ★ 중요
}

void CKey::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (CManagement::GetInstance()->Is_Paused())
		m_pTextureCom->Set_Texture(0);
	else
		m_pTextureCom->Set_Texture(m_iFrame_Key);
	
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CKey::OnCollision(CCollider* _pOther)
{
}

void CKey::OnCollisionEnter(CCollider* _pOther)
{
	OutputDebugStringA("[Key] OnCollisionEnter\n");

	if (m_bPicked || !_pOther) return;

	const auto layer = _pOther->Get_ColLayer();
	const bool allowed =
		(layer & COL_PLAYER);

	if (allowed == 0) return;


	if (CManagement::GetInstance()->Get_CurSceneId() == SCENEID::STAGE1)
	{
		vector<GameObjectSpawnInfo>loadMonsterData = CFileManager::GetInstance()->LoadAdditionalMonster((_uint)SCENEID::STAGE1, 0);

		for (auto monster : loadMonsterData)
			CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)monster.type1, (OBJID)monster.type2, monster.position);
	}

	m_bPicked = true;
	// 저역 키 보유 플래그 세팅 + 콜라이더 비활성
	CManagement::GetInstance()->Set_KeyOwned(true);
	if (m_pColliderCom)
		m_pColliderCom->Set_Active(false);
	OutputDebugStringA("[Key] picked, flag set\n");

	CStatsManager::I().OnItemPicked();
	CSoundManager::GetInstance()->PlaySoundW(L"DSITEMUP.wav", CHANNELID::SOUND_EFFECT, 0.8f);
}

void CKey::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CKey::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Key"));
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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Key\n");

	return S_OK;
}

void CKey::Set_OnTerrain()
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

void CKey::Bill_Board()
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

CKey* CKey::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKey* pKey = new CKey(pGraphicDev);

	return pKey;
}

CKey* CKey::Clone()
{
	CKey* pKey = new CKey(*this);

	if (FAILED(pKey->Ready_GameObject()))
	{
		Safe_Release(pKey);
		MSG_BOX("Key Create Failed");
		return nullptr;
	}

	return pKey;
}

void CKey::Free()
{
	Engine::CGameObject::Free();
}
