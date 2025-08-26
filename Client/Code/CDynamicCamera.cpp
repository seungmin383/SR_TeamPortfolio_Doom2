#include "pch.h"
#include "CDynamicCamera.h"
#include "CDInputManager.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CWeaponManager.h"
#include "CPistol.h"
#include "CShotgun.h"
#include "CChaingun.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CRocket.h"
#include "CBfg.h"
#include "CTerrainSector.h"

// Initialize static variable
_long CDynamicCamera::s_lMouseMoveX = 0L;

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev),
	m_bFix(false), m_bCheck(false), m_bTrigger(false), m_bIsMoving(false), m_bIsGrounded(true), m_bIsJumping(false),
	m_iArmor(0),
	m_fSpeed(0.f), m_fAngle(0.f), m_fAngleSpeed(0.f), m_fFallSpeed(0.f), m_fDist(0.f), m_fJumpForce(0.f),
	m_eState(IDLE),
	m_vMoveDist(_vec3(0.f, 0.f, 0.f))
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

	m_fSpeed = 20.f;
	m_fFallSpeed = 1.25f;
	m_fJumpForce = 0.45f;
	m_fAngle = 0.f;
	m_fAngleSpeed = 10.f;

	m_bDead = false;
	m_bIsMoving = false;

	if (FAILED(Engine::CCamera::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// Collider Default Settings
	_vec3 vScale{ 1.f, 3.f, 1.f };
	_vec3 vOffset{ 0.f, -1.5f, 0.f };

	m_pColliderCom->Ready_CCollider(this, m_vEye, vScale, vOffset, COL_PLAYER,
		COL_ENEMY | COL_OBJECT | COL_TERRAIN | COL_WALL | COL_ITEM | COL_WEAPON);

	CWeapon* pPistol = CPistol::Create();
	if (pPistol != nullptr)
		CWeaponManager::GetInstance()->Add_Weapon(WEAPON_ID::PISTOL, pPistol);

	CWeaponManager::GetInstance()->Set_CurWeapon(WEAPON_ID::PISTOL);

	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);
	
	// 일시 정지 기능 추가
	if (CManagement::GetInstance()->Is_Paused())
		return 0;

	Update_State();

	return iExit;
}

void CDynamicCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::CCamera::LateUpdate_GameObject(fTimeDelta);
	
	Apply_Gravity(fTimeDelta);
	Set_OnTerrain();
	Key_Input(fTimeDelta);
	Mouse_Input();

	// 콜라이더와 위치를 옮겨주는 부분
	m_pColliderCom->Late_Update();
	m_pTransformCom->Set_Pos(m_vEye.x, m_vEye.y, m_vEye.z);

	// 마우스 부분
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

void CDynamicCamera::OnCollision(CCollider* _pOther)
{
	if (_pOther->Get_ColLayer() == COL_ENEMY && CCollisionManager::Check_AABB(m_pColliderCom, _pOther))
	{
		CCollisionManager::Collision_Rect_Resolve(_pOther, m_pColliderCom, CTimerManager::GetInstance()->Get_TimeDelta(L"Timer_60"));

		_vec3 curPos;
		m_pTransformCom->Get_Pos(&curPos);
		m_pTransformCom->Set_Pos(curPos.x + (-m_vMoveDist.x), curPos.y, curPos.z + (-m_vMoveDist.z));

		m_vEye.x += (-m_vMoveDist.x);
		m_vEye.z += (-m_vMoveDist.z);

		m_vAt.x += (-m_vMoveDist.x);
		m_vAt.z += (-m_vMoveDist.z);

		m_vMoveDist = _vec3(0.f, 0.f, 0.f);
	}
}

void CDynamicCamera::OnCollisionEnter(CCollider* _pOther)
{
	if (_pOther->Get_ColLayer() == COL_ENEMY && CCollisionManager::Check_AABB(m_pColliderCom, _pOther))
	{
		CCollisionManager::Collision_Rect_Resolve(_pOther, m_pColliderCom, CTimerManager::GetInstance()->Get_TimeDelta(L"Timer_60"));

		_vec3 curPos;
		m_pTransformCom->Get_Pos(&curPos);
		m_pTransformCom->Set_Pos(curPos.x + (-m_vMoveDist.x), curPos.y, curPos.z + (-m_vMoveDist.z));

		m_vEye.x += (-m_vMoveDist.x);
		m_vEye.z += (-m_vMoveDist.z);

		m_vAt.x += (-m_vMoveDist.x);
		m_vAt.z += (-m_vMoveDist.z);


		m_vMoveDist = _vec3(0.f, 0.f, 0.f);
	}
}

void CDynamicCamera::OnCollisionExit(CCollider* _pOther)
{
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

	return S_OK;
}

void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	m_bIsMoving = false;

	_matrix	matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);
#pragma region Moving Part

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vMoveDist = vLength;

		m_vEye.x += vLength.x;
		m_vEye.z += vLength.z;

		m_vAt.x += vLength.x;
		m_vAt.z += vLength.z;
	
		m_bIsMoving = true;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vMoveDist = -vLength;

		m_vEye.x -= vLength.x;
		m_vEye.z -= vLength.z;

		m_vAt.x -= vLength.x;
		m_vAt.z -= vLength.z;

		m_bIsMoving = true;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vMoveDist = vLength;

		m_vEye.x += vLength.x;
		m_vEye.z += vLength.z;

		m_vAt.x += vLength.x;
		m_vAt.z += vLength.z;

		m_bIsMoving = true;
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vMoveDist = -vLength;

		m_vEye.x -= vLength.x;
		m_vEye.z -= vLength.z;

		m_vAt.x -= vLength.x;
		m_vAt.z -= vLength.z;

		m_bIsMoving = true;
	}

#pragma endregion

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		if (m_bIsGrounded)
		{
			m_bIsJumping = true;
			m_bIsGrounded = false;
			m_vVelocity.y -= m_fJumpForce;
		}
		else
			return;
	}

	// 움직이고 있다면 카메라 흔들림 효과 적용
	if (m_bIsMoving)
		Camera_Bobbing(fTimeDelta);

#pragma region	Weapon Change Part
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

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_4) & 0x80)
	{
		CWeaponManager::GetInstance()->Change_Weapon(WEAPON_ID::ROCKET);
	}

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_5) & 0x80)
	{
		CWeaponManager::GetInstance()->Change_Weapon(WEAPON_ID::BFG);
	}
#pragma endregion

#pragma region Mouse Fix Part
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
#pragma endregion
}

void CDynamicCamera::Mouse_Input()
{
	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
		_bool bFireCheck = CWeaponManager::GetInstance()->Fire_Weapon();

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
	// 소재혁 지형 변경 2025-08-26
	// TODO:로딩 시 지형정보 들고있게 수정
	// TODO:최적화
	_float fHighest(-999.f);
	_vec3 onPosition{ 0.f, 0.f, 0.f };

	// 플레이어 바로 아래의 Terrain 중 가장 높은 Terrain의 높이를 반환 --> 복층일 때 계산할 수 있어야하므로
	for (auto terrain : CManagement::GetInstance()->Get_Scene()->Get_Terrain())
	{
		auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();

		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &m_vEye))
		{
			if (fHighest < onPosition.y)
				fHighest = onPosition.y;
		}
	}

	// 예외처리용 코드 : 변수가 바뀌지 않았다는건 바로 아래 지형이 없었다는 뜻
	if (fHighest == -999.f)
		fHighest = 0.f;

	if (!m_bIsJumping)
	{
		m_bIsGrounded = true;
		_float deltaHeight = (fHighest + 3.f) - m_vEye.y;
		m_vEye.y += deltaHeight;
		m_vAt.y += deltaHeight;
	}

	// 점프 중 일때 착지 로직
	if (m_bIsJumping && (m_vEye.y - 3.f) <= fHighest)
	{
		m_bIsJumping = false;

		_float deltaHeight = (fHighest + 3.f) - m_vEye.y;
		m_vEye.y += deltaHeight;
		m_vAt.y += deltaHeight;

		m_vVelocity.y = 0.f;
		m_bIsGrounded = true;
	}
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

void CDynamicCamera::Camera_Bobbing(const _float& fTimeDelta)
{
	// 카메라 위, 아래 효과
	if (!m_bIsGrounded)
		return;

	m_fAngle += m_fAngleSpeed * fTimeDelta;
	_float fBobAngle = sinf(m_fAngle) * 0.2f;
	m_vEye.y += fBobAngle;
	m_vAt.y += fBobAngle;
}

void CDynamicCamera::Apply_Gravity(const _float& fTimeDelta)
{
	if (!m_bIsGrounded)
	{
		m_vVelocity.y += m_fFallSpeed * fTimeDelta;
		m_vEye.y -= m_vVelocity.y;
		m_vAt.y -= m_vVelocity.y;
	}
	else
		m_vVelocity.y = 0.f;

	if (m_vVelocity.y >= 200.f)
		m_vVelocity.y = 200.f;
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
