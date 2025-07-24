#include "pch.h"
#include "CPlayer.h"
#include "CPrototypeManager.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_fLookX(0.f)
{
}

CPlayer::CPlayer(const CGameObject& rhs)
	: CGameObject(rhs),
	m_fLookX(0.f)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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

	//Camera
	pComponent = m_pCameraCom = dynamic_cast<Engine::CCamera*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_PlayerCamera"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Camera", pComponent });

	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	if (GetAsyncKeyState(VK_UP))
	{
		_vec3 vUp;
		m_pTransformCom->Get_Info(INFO_LOOK, &vUp);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vUp, &vUp), 3.f, fTimeDelta);

		_vec3 vRight;
		m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), m_fLookX > 0 ? 3.f : -3.f, fTimeDelta);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		_vec3 vDown;
		m_pTransformCom->Get_Info(INFO_LOOK, &vDown);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vDown, &vDown), -3.f, fTimeDelta);

		_vec3 vRight;
		m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), m_fLookX > 0 ? 3.f : -3.f, fTimeDelta);
	}
	//if (GetAsyncKeyState(VK_LEFT))
	//{
	//	_vec3 vLeft;
	//	m_pTransformCom->Get_Info(INFO_RIGHT, &vLeft);
	//	m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLeft, &vLeft), -3.f, fTimeDelta);
	//}
	//if (GetAsyncKeyState(VK_RIGHT))
	//{
	//	_vec3 vRight;
	//	m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
	//	m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), m_fLookX, fTimeDelta);
	//}

	//if (GetAsyncKeyState('W'))
	//{
	//	m_pTransformCom->Rotation(ROT_X, -1.f * fTimeDelta);
	//}
	//if (GetAsyncKeyState('S'))
	//{
	//	m_pTransformCom->Rotation(ROT_X, 1.f * fTimeDelta);
	//}
	//if (GetAsyncKeyState('A'))
	//{
	//	m_pTransformCom->Rotation(ROT_Y, 1.f * fTimeDelta);
	//}
	//if (GetAsyncKeyState('D'))
	//{
	//	m_pTransformCom->Rotation(ROT_Z, 1.f * fTimeDelta);
	//}

	if (GetAsyncKeyState('Q'))
	{
		m_pTransformCom->Rotation(ROT_X, D3DXToRadian(180.f * fTimeDelta));
	}

	_vec3 pos;
	m_pTransformCom->Get_Info(INFO_POS, &pos);
	m_pCameraCom->Chase_Target(&pos);

	POINT point;
	GetCursorPos(&point);
	ScreenToClient(g_hWnd, &point);

	if (point.x > (WINCX >> 1))
	{
		m_fLookX += 0.1f;
		m_pTransformCom->Rotation(ROT_Y, 0.5f * fTimeDelta);
	}
	else if (point.x < (WINCX >> 1))
	{
		m_fLookX -= 0.1f;
		m_pTransformCom->Rotation(ROT_Y, -0.5f * fTimeDelta);
	}

	if (m_fLookX > 5.f)
	{
		m_fLookX = 5.f;
	}
	else if (m_fLookX < -5.f)
	{
		m_fLookX = -5.f;
	}

	if (point.y > (WINCY >> 1))
	{
		m_fLookY -= 0.1f;
		m_pTransformCom->Rotation(ROT_X, 0.5f * fTimeDelta);
	}
	else if (point.y < (WINCY >> 1))
	{
		m_fLookY += 0.1f;
		m_pTransformCom->Rotation(ROT_X, -0.5f * fTimeDelta);
	}

	if (m_fLookY > 5.f)
	{
		m_fLookY = 5.f;
	}
	else if (m_fLookY < -5.f)
	{
		m_fLookY = -5.f;
	}

	m_pCameraCom->Look_AroundX(m_fLookX);
	m_pCameraCom->Look_AroundY(m_fLookY);
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

