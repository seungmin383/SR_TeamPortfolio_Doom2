#include "pch.h"
#include "CZombieMan.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"

CZombieMan::CZombieMan(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonsterBase(pGraphicDev)
{
}

CZombieMan::CZombieMan(const CMonsterBase& rhs)
	: CMonsterBase(rhs)
{
}

CZombieMan::~CZombieMan()
{
}

HRESULT CZombieMan::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(1.f, 1.f, 1.f), _vec3(1.f, 1.f, 1.f),
		COLLAYERID::COL_ENEMY, 
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT);

	return S_OK;
}

_int CZombieMan::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	Set_OnTerrain();
	Bill_Board();
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return 0;
}

void CZombieMan::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	CGameObject::LateUpdate_GameObject(fTimeDelta);

	m_pColliderCom->Late_Update();
}

void CZombieMan::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CZombieMan::OnCollision(CCollider* _pOther)
{
}

void CZombieMan::OnCollisionEnter(CCollider* _pOther)
{
}

void CZombieMan::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CZombieMan::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_ZombieMan_Texture"));
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

void CZombieMan::Key_Input(const _float& fTimeDelta)
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

void CZombieMan::Set_OnTerrain()
{
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	Engine::CTerrainTex* pTerrainBufferCom =
		static_cast<CTerrainTex*>
		(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));

	_float height = m_pCalculatorCom->Compute_HeightOnTerrain(&pos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_pTransformCom->Set_Pos(pos.x, height + 2.f, pos.z);
}

CZombieMan* CZombieMan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CZombieMan* tmp = new CZombieMan(pGraphicDev);

	if (FAILED(tmp->Ready_GameObject()))
	{
		Safe_Release(tmp);
		MSG_BOX("Player Create Failed");
		return nullptr;
	}

	return tmp;
}

void CZombieMan::Free()
{
	Engine::CGameObject::Free();
}

void CZombieMan::Manage_MoveState()
{

}

void CZombieMan::Manage_AnimState()
{

}

void CZombieMan::Movement()
{
}

void CZombieMan::Attack()
{
}

void CZombieMan::Bill_Board()
{
	_matrix	matWorld, matView, matBill;

	m_pTransformCom->Get_World(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	// 뷰 스페이스 -> 카메라 월드 행렬의 역행렬

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);		// 카메라가 갖고 있는 y회전의 역행렬 -역행렬> 카메라의 월드 상태의 y축 회전 행렬

	matWorld = matBill * matWorld;

	m_pTransformCom->Set_World(&matWorld);

}

