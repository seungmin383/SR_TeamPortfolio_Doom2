#include "pch.h"
#include "CDynamicCamera.h"
#include "CDInputManager.h"
#include "CManagement.h"

// 변수 초기화
_long CDynamicCamera::s_lMouseMoveX = 0L;

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev),
	m_bFix(false), m_bCheck(false),
	m_iHp(0), m_iArmor(0),
	m_fSpeed(0.f)
{
}

CDynamicCamera::~CDynamicCamera()
{
}

HRESULT CDynamicCamera::Ready_GameObject(const _vec3* pEye,
											const _vec3* pAt,
											const _vec3* pUp,
											const _float& fFov,
											const _float& fAspect,
											const _float& fNear,
											const _float& fFar)
{
	// Camera Default Settings
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	// Player Default Settings
	m_iHp = 100;
	m_iArmor = 50;

	m_fSpeed = 10.f;

	m_bDead = false;

	if (FAILED(Engine::CCamera::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// Collider Default Settings
	_vec3 vScale{ 20.f, 20.f, 20.f };
	_vec3 vOffset{ 0.f, 0.f, 0.f };

	m_pColliderCom->Ready_CCollider(this, m_vEye, vScale, vOffset, COL_PLAYER,
		COL_ENEMY | COL_OBJECT | COL_TERRAIN | COL_WALL);

	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);
	
	Set_OnTerrain();

	return iExit;
}

void CDynamicCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::CCamera::LateUpdate_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);

	m_pColliderCom->Late_Update();

	if (false == m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}
}

void CDynamicCamera::Render_GameObject()
{
}

HRESULT CDynamicCamera::Add_Component()
{
	CComponent* pComponent = NULL;

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	return S_OK;

}

void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	_matrix	matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye.x += vLength.x;
		m_vEye.z += vLength.z;

		m_vAt.x += vLength.x;
		m_vAt.z += vLength.z;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye.x -= vLength.x;
		m_vEye.z -= vLength.z;

		m_vAt.x -= vLength.x;
		m_vAt.z -= vLength.z;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye.x += vLength.x;
		m_vEye.z += vLength.z;

		m_vAt.x += vLength.x;
		m_vAt.z += vLength.z;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye.x -= vLength.x;
		m_vEye.z -= vLength.z;

		m_vAt.x -= vLength.x;
		m_vAt.z -= vLength.z;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_TAB) & 0x80)
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;

		else
			m_bFix = true;
	}
	else
	{
		m_bCheck = false;
	}

	if (false == m_bFix)
		return;
}

void CDynamicCamera::Mouse_Move()
{
	_matrix	matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	_long  dwMouseMove = 0L;

	if (dwMouseMove = CDInputManager::GetInstance()->Get_DIMouseMove(DIMS_Y))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLook = m_vAt - m_vEye;
		_matrix matRot;

		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}

	// 마우스 x축 이동 값을 저장.(매 프레임마다 최신 마우스 이동 값을 어디서든 확인 가능)
	s_lMouseMoveX = CDInputManager::GetInstance()->Get_DIMouseMove(DIMS_X);
	if (s_lMouseMoveX)
	{
		_vec3	vUp{ 0.f, 1.f, 0.f };

		_vec3	vLook = m_vAt - m_vEye;
		_matrix matRot;

		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(s_lMouseMoveX / 10.f));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}
}

void CDynamicCamera::Mouse_Fix()
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CDynamicCamera::Set_OnTerrain()
{
	// 지형의 버텍스 정보를 가져온다
	Engine::CTerrainTex* pTerrainBufferCom =
		static_cast<CTerrainTex*>
		(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));

	_float height = m_pCalculatorCom->Compute_HeightOnTerrain(&m_vEye, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);
	
	m_vEye.y = height + 2.f;
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CDynamicCamera* pCamera = new CDynamicCamera(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pCamera);
		MSG_BOX("DynamicCamera Create Failed");
		return nullptr;
	}

	return pCamera;
}

void CDynamicCamera::Free()
{
	Engine::CCamera::Free();
}
