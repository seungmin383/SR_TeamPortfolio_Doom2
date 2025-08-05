#include "pch.h"
#include "CPlayer.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CPlayer::CPlayer(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	//TODO: 2025-08-03 Layer 단계에서 게임오브젝트 생성 시 스케일 등등 셋팅할 수 있게 수정

	m_pTransformCom->Set_Pos(3.f, 3.f, 3.f);

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_PLAYER, COLLAYERID::COL_ENEMY | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT);

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	Set_OnTerrain();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	m_pColliderCom->Late_Update();
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	//Rend Collider
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CPlayer::OnCollision(CCollider* _pOther)
{
}

void CPlayer::OnCollisionEnter(CCollider* _pOther)
{
}

void CPlayer::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CPlayer::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_PlayerTexture"));
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

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	_vec3 vLook;
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);

	if (GetAsyncKeyState(VK_UP))
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), 3.f, fTimeDelta);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), -3.f, fTimeDelta);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-180.f * fTimeDelta));
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f * fTimeDelta));
	}
}

void CPlayer::Set_OnTerrain()
{
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	Engine::CTerrainTex* pTerrainBufferCom = 
		static_cast<CTerrainTex*>
		(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));

	_float height = m_pCalculatorCom->Compute_HeightOnTerrain(&pos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_pTransformCom->Set_Pos(pos.x, height + 2.f, pos.z);
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("Player Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CPlayer::Free()
{
	Engine::CGameObject::Free();
}

