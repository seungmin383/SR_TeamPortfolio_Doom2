#include "pch.h"
#include "CMonster.h"
#include "CPrototypeManager.h"

#include "CManagement.h"
#include "CRenderer.h"

#include "CLogo.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMonster::CMonster(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return 0;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Chase_Player(fTimeDelta);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMonster::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonster::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CTriCol*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_TriCol"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

void CMonster::Chase_Player(const _float& fTimeDelta)
{
	auto pManagementClass = CManagement::GetInstance();
	auto playerTransCom = pManagementClass->Get_Component(ID_DYNAMIC, LAYERID::LAYER_GAMELOGIC, OBJID::OBJ_PLAYER, L"Com_Transform");

	/*_vec3 infoRight;
	dynamic_cast<CTransform*>(playerTransCom)->Get_Info(INFO_RIGHT, &infoRight);

	_vec3 infoUp;
	dynamic_cast<CTransform*>(playerTransCom)->Get_Info(INFO_UP, &infoUp);

	m_pTransformCom->Move_Pos(D3DXVec3Normalize(&infoRight, &infoRight), 1.f, fTimeDelta);

	m_pTransformCom->Move_Pos(D3DXVec3Normalize(&infoUp, &infoUp), 1.f, fTimeDelta);*/

	_vec3 playerPos;
	_vec3 myPos;
	_vec3 vDiff;
	_vec3 vLook;
	dynamic_cast<CTransform*>(playerTransCom)->Get_Pos(&playerPos);
	m_pTransformCom->Get_Pos(&myPos);
	vDiff = playerPos - myPos;
	
	m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vDiff, &vDiff), 1.f, fTimeDelta);

	m_pTransformCom->Set_TargetPos(playerPos);

	//dynamic_cast<CTransform*>(playerTransCom)->Get_Info(INFO_LOOK, &vLook);
	//m_pTransformCom->Add_Info(INFO_LOOK, vDiff);

	//m_pTransformCom->Set_Dir(vLook);
	//m_pTransformCom->Set_Angle(vDiff);

	//_vec3 vAngle;
	//dynamic_cast<CTransform*>(playerTransCom)->Get_Angle(&vAngle);
	
	//_vec3 vRotAxis = { 0.f, 0.f, 0.f };
	//_matrix matRotX;
	//_matrix matRotY;
	//_matrix matRotZ;
	//D3DXMatrixRotationAxis(&matRotX, &vRotAxis, vAngle.x);
	//D3DXMatrixRotationAxis(&matRotY, &vRotAxis, vAngle.y);
	//D3DXMatrixRotationAxis(&matRotZ, &vRotAxis, vAngle.z);
}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonster* pMonster = new CMonster(pGraphicDev);

	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("Monster Create Failed");
		return nullptr;
	}

	return pMonster;
}

void CMonster::Free()
{
	Engine::CGameObject::Free();
}

