#include "pch.h"
#include "CDynamicCamera.h"
#include "CDInputManager.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CWeaponManager.h"
#include "CPistol.h"
#include "CShotgun.h"
#include "CChaingun.h"

// Initialize static variable
_long CDynamicCamera::s_lMouseMoveX = 0L;

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev),
	m_bFix(false), m_bCheck(false), m_bTrigger(false),
	m_iGameHp(0), m_iArmor(0),
	m_fSpeed(0.f),
	m_eState(IDLE)
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
	m_iGameHp = 83;
	m_iArmor = 45;

	m_fSpeed = 40.f;

	m_bDead = false;

	if (FAILED(Engine::CCamera::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// Collider Default Settings
	_vec3 vScale{ 1.f, 1.f, 1.f };
	_vec3 vOffset{ 0.f, 0.f, 0.f };

	m_pColliderCom->Ready_CCollider(this, m_vEye, vScale, vOffset, COL_PLAYER,
		COL_ENEMY | COL_OBJECT | COL_TERRAIN | COL_WALL | COL_ITEM);

	CWeapon* pPistol = CPistol::Create();
	if (pPistol != nullptr)
		CWeaponManager::GetInstance()->Add_Weapon(WEAPON_ID::PISTOL, pPistol);

	// 테스트용 무기 추가 나중에 삭제 해야됌
	CWeapon* pShotgun = CShotgun::Create();
	if (pShotgun != nullptr)
		CWeaponManager::GetInstance()->Add_Weapon(WEAPON_ID::SHOTGUN, pShotgun);

	CWeapon* pChaingun = CChaingun::Create();
	if (pChaingun != nullptr)
		CWeaponManager::GetInstance()->Add_Weapon(WEAPON_ID::CHAINGUN, pChaingun);

	CWeaponManager::GetInstance()->Set_CurWeapon(WEAPON_ID::PISTOL);

	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);
	
	Update_State();
	//Set_OnTerrain();

	return iExit;
}

void CDynamicCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::CCamera::LateUpdate_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);
	//Mouse_Input();

	m_pColliderCom->Late_Update();

	if (false == m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
}

void CDynamicCamera::Render_GameObject(const _float& fTimeDelta)
{
	m_pColliderCom->Render(g_colRend);
}

HRESULT CDynamicCamera::Add_Component()
{
	CComponent* pComponent = NULL;

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	OutputDebugStringA("[Stage] add OBJ_PLAYERCAMERA: DynamicCamera\n");

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
		m_vEye.y += vLength.y;
		m_vEye.z += vLength.z;

		m_vAt.x += vLength.x;
		m_vAt.y += vLength.y;
		m_vAt.z += vLength.z;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye.x -= vLength.x;
		m_vEye.y -= vLength.y;
		m_vEye.z -= vLength.z;

		m_vAt.x -= vLength.x;
		m_vAt.y -= vLength.y;
		m_vAt.z -= vLength.z;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye.x += vLength.x;
		m_vEye.y += vLength.y;
		m_vEye.z += vLength.z;

		m_vAt.x += vLength.x;
		m_vAt.y += vLength.y;
		m_vAt.z += vLength.z;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye.x -= vLength.x;
		m_vEye.y -= vLength.y;
		m_vEye.z -= vLength.z;

		m_vAt.x -= vLength.x;
		m_vAt.y -= vLength.y;
		m_vAt.z -= vLength.z;
	}

	// 무기 교체 관련 키 입력
	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_1) & 0x80)
	{
		CWeaponManager::GetInstance()->Change_Weapon(WEAPON_ID::PISTOL);
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_2) & 0x80)
	{
		CWeaponManager::GetInstance()->Change_Weapon(WEAPON_ID::SHOTGUN);
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_3) & 0x80)
	{
		CWeaponManager::GetInstance()->Change_Weapon(WEAPON_ID::CHAINGUN);
	}


	//m_pTransformCom 업데이트 //collider 동기화 용
	m_pTransformCom->Set_Pos(m_vEye.x, m_vEye.y, m_vEye.z);

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

void CDynamicCamera::Mouse_Input()
{
	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
	{
		_bool bFireCheck = CWeaponManager::GetInstance()->Fire_Weapon();

		if (bFireCheck)
			m_pCalculatorCom->Picking_GameObject(g_hWnd);
	}

	m_bTrigger = false;

	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_RB) & 0x80)
	{
		m_bTrigger = true;
	}
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
	Engine::CTerrainTexSector* pTerrainBufferCom =
		static_cast<CTerrainTexSector*>
		(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));

	_float height = m_pCalculatorCom->Compute_HeightOnTerrain(&m_vEye, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);
	
	// 카메라의 높이 (m_vEye.y)가 변화할 때 마다 바라보는 목표(m_vAt.y)도 바뀌어야한다.
	// 카메라의 높이 변화량을 계산해서 변화량을 m_vEye, m_vAt 모두 더해줘야 시점이 유지된다.
	_float deltaHeight = (height + 2.f) - m_vEye.y;
	m_vEye.y += deltaHeight;
	m_vAt.y += deltaHeight;
}

void CDynamicCamera::Update_State()
{
	if (m_iGameHp >= 80)
		m_eState = IDLE;
	else if (m_iGameHp <= 79 && m_iGameHp >= 60)
		m_eState = HURT1;
	else if (m_iGameHp <= 59 && m_iGameHp >= 40)
		m_eState = HURT2;
	else if (m_iGameHp <= 39 && m_iGameHp >= 20)
		m_eState = HURT3;
	else if (m_iGameHp <= 19 && m_iGameHp >= 1)
		m_eState = HURT4;
	else
	{
		m_eState = DEAD;
		m_bDead = true;
	}
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
