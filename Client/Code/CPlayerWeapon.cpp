#include "pch.h"
#include "CPlayerWeapon.h"
#include "CDInputManager.h"
#include "CRenderer.h"
#include "CWeaponManager.h"

CPlayerWeapon::CPlayerWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev),
	m_eCurWeaponId(WEAPON_ID::ID_END), m_eWeaponState(WEAPON_STATE::STATE_END), m_eNextWeaponId(WEAPON_ID::ID_END),
	m_fSwapTime(0.f), m_fBobSpeed(0.f), m_fBobAngle(0.f),
	m_bIsMoving(false), m_bBobForward(false)
{
	ZeroMemory(&m_vScale, sizeof(_vec3));
	ZeroMemory(&m_vPos, sizeof(_vec3));
	ZeroMemory(&m_vBobPos, sizeof(_vec3));
}

CPlayerWeapon::CPlayerWeapon(const CCharacter& rhs)
	: CCharacter(rhs)
{
}

CPlayerWeapon::~CPlayerWeapon()
{
}

HRESULT CPlayerWeapon::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_bBobForward = true;
	m_fBobSpeed = 3.5f;
	m_fBobAngle = 0.f;
	m_vScale = { 300.f, 300.f, 1.f };
	m_vPos = { 0.f, -WINCY * 0.5f + m_vScale.y , 1.f };
	
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Get_Pos(&m_vBobPos);

	//애니메이션 세팅
	Ready_Animation();

	return S_OK;
}

_int CPlayerWeapon::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	m_eCurWeaponId = CWeaponManager::GetInstance()->Get_CurWeapon()->Get_WeaponID();
	m_eWeaponState = CWeaponManager::GetInstance()->Get_CurWeapon()->Get_WeaponState();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CPlayerWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);
	Weapon_Bobbing(m_bIsMoving, fTimeDelta);

	if (m_eWeaponState == WEAPON_STATE::SWAP)
		Weapon_Swap(fTimeDelta);

	// 사격 상태일 때 가운데로 위치 조정
	if (m_eWeaponState == WEAPON_STATE::SHOT)
		m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	m_pAnimatorCom->Play_Animation((_uint)m_eCurWeaponId + (_uint)m_eWeaponState, 0, fTimeDelta);

	// 디버깅용 코드
	//wchar_t buf[64];
	//swprintf(buf, 64, L"State : %d\n", m_eWeaponState);
	//OutputDebugString(buf);
}

void CPlayerWeapon::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture((_uint)m_eCurWeaponId);
	
	// 알파블렌딩 설정
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pBufferCom->Render_Buffer();

	// 렌더 상태 복원
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CPlayerWeapon::Ready_Animation()
{
	// Pistol Animation
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::PISTOL, m_pTextureCom, m_pBufferCom, 5, 1, 0, 0, 0.f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::PISTOL + (_uint)WEAPON_STATE::SWAP, m_pTextureCom, m_pBufferCom, 5, 1, 0, 0, 0.1f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::PISTOL + (_uint)WEAPON_STATE::SHOT, m_pTextureCom, m_pBufferCom, 5, 1, 0, 4, 0.1f, true);
	
	// Shotgun Animation
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::SHOTGUN, m_pTextureCom, m_pBufferCom, 8, 1, 0, 0, 0.f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::SHOTGUN + (_uint)WEAPON_STATE::SWAP, m_pTextureCom, m_pBufferCom, 8, 1, 0, 0, 0.2f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::SHOTGUN + (_uint)WEAPON_STATE::SHOT, m_pTextureCom, m_pBufferCom, 8, 1, 0, 7, 0.11643f, true);

	// Chaingun Animation
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::CHAINGUN, m_pTextureCom, m_pBufferCom, 3, 1, 0, 0, 0.f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::CHAINGUN + (_uint)WEAPON_STATE::SWAP, m_pTextureCom, m_pBufferCom, 3, 1, 0, 0, 0.1f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::CHAINGUN + (_uint)WEAPON_STATE::SHOT, m_pTextureCom, m_pBufferCom, 3, 1, 1, 2, 0.057f, true);

	// Rocket Animation
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::ROCKET, m_pTextureCom, m_pBufferCom, 6, 1, 0, 0, 0.f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::ROCKET + (_uint)WEAPON_STATE::SWAP, m_pTextureCom, m_pBufferCom, 6, 1, 0, 0, 0.1f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::ROCKET + (_uint)WEAPON_STATE::SHOT, m_pTextureCom, m_pBufferCom, 6, 1, 1, 5, 0.15f, false);

	// BFG9000 Animation
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::BFG, m_pTextureCom, m_pBufferCom, 3, 1, 0, 0, 0.f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::BFG + (_uint)WEAPON_STATE::SWAP, m_pTextureCom, m_pBufferCom, 3, 1, 0, 0, 0.1f, true);
	m_pAnimatorCom->Add_Animation((_uint)WEAPON_ID::BFG + (_uint)WEAPON_STATE::SHOT, m_pTextureCom, m_pBufferCom, 3, 1, 0, 2, 0.38f, true);
}

HRESULT CPlayerWeapon::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_PlayerWeaponTexture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	//Animator 애니메이션 세팅
	pComponent = m_pAnimatorCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CPlayerWeapon::Key_Input(const _float& fTimeDelta)
{
	// 이동 관련 키 입력
	m_bIsMoving = false;

	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_W) & 0x80 ||
		CDInputManager::GetInstance()->Get_DIKeyState(DIK_A) & 0x80 ||
		CDInputManager::GetInstance()->Get_DIKeyState(DIK_S) & 0x80 ||
		CDInputManager::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_bIsMoving = true;
	}
}

void CPlayerWeapon::Weapon_Swap(const _float& fTimeDelta)
{
	_vec3 vUp = { 0.f, 1.f, 0.f };
	_vec3 vDown = { 0.f, -1.f, 0.f };

	m_fSwapTime = CWeaponManager::GetInstance()->Get_SwapTime();

	if (m_fSwapTime < 0.5f)
		m_pTransformCom->Move_Pos(&vDown, 700.f, fTimeDelta);
	else if (m_fSwapTime >= 0.5f && m_fSwapTime < 1.f)
		m_pTransformCom->Move_Pos(&vUp, 700.f, fTimeDelta);
	else
	{
		m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
		return;
	}
}

void CPlayerWeapon::Weapon_Bobbing(bool bIsMoving, const _float& fTimeDelta)
{
	// 무기 교체 중이거나 사격 중이라면 리턴
	if (m_eWeaponState == WEAPON_STATE::SWAP || m_eWeaponState == WEAPON_STATE::SHOT)
		return;

	// 플레이어가 이동 중일 때
	if (bIsMoving)
	{
		// 변수인 각도가 더해져야 할 경우와 뺄 경우를 나눈다.
		if (m_bBobForward)
			m_fBobAngle += m_fBobSpeed * fTimeDelta;
		else
			m_fBobAngle -= m_fBobSpeed * fTimeDelta;

		// 각도가 특정 각 이상이면 변수의 진행 방향을 바꿔준다.
		if (m_fBobAngle >= D3DX_PI * 1.5f)
		{
			m_fBobAngle = D3DX_PI * 1.5f;
			m_bBobForward = false;
		}
		else if (m_fBobAngle <= D3DX_PI * 0.5f)
		{
			m_fBobAngle = D3DX_PI * 0.5f;
			m_bBobForward = true;
		}

		// 위에서 구한 각도를 이용하여 수직, 수평 이동량을 구한다.
		_float fVerticalBob = sinf(m_fBobAngle) * 2.5f;
		_float fHorizontalBob = m_vPos.y - cosf(m_fBobAngle) * 25.f;

		m_vBobPos.x += fVerticalBob;
		m_vBobPos.y = fHorizontalBob;

		m_pTransformCom->Set_Pos(m_vBobPos.x, m_vBobPos.y, m_vBobPos.z);
	}
	else
	{
		// 이동을 멈춘다면 선형 보간을 하여 처음 위치로 초기화
		D3DXVec3Lerp(&m_vBobPos, &m_vBobPos, &m_vPos, 7.f * fTimeDelta);
		m_pTransformCom->Set_Pos(m_vBobPos.x, m_vBobPos.y, m_vBobPos.z);
		m_fBobAngle = 0.f;
	}
}


CPlayerWeapon* CPlayerWeapon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerWeapon* pWeapon = new CPlayerWeapon(pGraphicDev);

	if (FAILED(pWeapon->Ready_GameObject()))
	{
		Safe_Release(pWeapon);
		MSG_BOX("Weapon Clone Failed");
		return pWeapon;
	}

	return pWeapon;
}

CPlayerWeapon* CPlayerWeapon::Clone()
{
	CPlayerWeapon* pWeapon = new CPlayerWeapon(*this);

	return pWeapon;
}

void CPlayerWeapon::Free()
{
	Engine::CGameObject::Free();
}
