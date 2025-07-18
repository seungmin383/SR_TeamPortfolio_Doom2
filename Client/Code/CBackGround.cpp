#include "pch.h"
#include "CBackGround.h"
#include "CPrototypeManager.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CBackGround::CBackGround(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CBackGround::~CBackGround()
{
}

HRESULT CBackGround::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CBackGround::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CBackGround::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBackGround::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pBufferCom->Render_Buffer();
	m_pMonsterBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBackGround::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CTriCol*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_TriCol"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	
	//////
	pComponent = m_pMonsterBufferCom = dynamic_cast<Engine::CRectCol*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_RectCol"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Monster_Com_Buffer", pComponent });

	pComponent = m_pMonsterTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_RectTransform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Monster_Com_Transform", pComponent });
	//////

	return S_OK;
}

void CBackGround::Key_Input(const _float& fTimeDelta)
{
	if (GetAsyncKeyState(VK_UP))
	{
		_vec3 vUp;
		m_pTransformCom->Get_Info(INFO_UP, &vUp);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vUp, &vUp), 3.f, fTimeDelta);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		_vec3 vDown;
		m_pTransformCom->Get_Info(INFO_UP, &vDown);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vDown, &vDown), -3.f, fTimeDelta);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		_vec3 vLeft;
		m_pTransformCom->Get_Info(INFO_RIGHT, &vLeft);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLeft, &vLeft), -3.f, fTimeDelta);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		_vec3 vRight;
		m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), 3.f, fTimeDelta);
	}

	if (GetAsyncKeyState('W'))
	{
		m_pTransformCom->Rotation(ROT_X, -1.f * fTimeDelta);
	}
	if (GetAsyncKeyState('S'))
	{
		m_pTransformCom->Rotation(ROT_X, 1.f * fTimeDelta);
	}
	if (GetAsyncKeyState('A'))
	{
		m_pTransformCom->Rotation(ROT_Y, 1.f * fTimeDelta);
	}
	if (GetAsyncKeyState('D'))
	{
		m_pTransformCom->Rotation(ROT_Z, 1.f * fTimeDelta);
	}

	if (GetAsyncKeyState('Q'))
	{
		m_pTransformCom->Rotation(ROT_X, D3DXToRadian(180.f * fTimeDelta));
	}
}

CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBackGround* pBackGround = new CBackGround(pGraphicDev);

	if (FAILED(pBackGround->Ready_GameObject()))
	{
		Safe_Release(pBackGround);
		MSG_BOX("BackGround Create Failed");
		return nullptr;
	}

	return pBackGround;
}

void CBackGround::Free()
{
	Engine::CGameObject::Free();
}
