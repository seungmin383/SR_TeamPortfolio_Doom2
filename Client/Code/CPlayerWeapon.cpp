#include "pch.h"
#include "CPlayerWeapon.h"
#include "CDInputManager.h"
#include "CRenderer.h"

CPlayerWeapon::CPlayerWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev),
	m_eWeaponId(PISTOL), m_eWeaponState(IDLE)
{
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

	_vec3 vPos, vScale, vSize;
	
	vScale = { 300.f, 300.f, 1.f };
	vPos = { 0.f, -WINCY * 0.5f + vScale.y , 1.f };
	
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Set_Scale(vScale);

	//애니메이션 세팅
	Ready_Animation();

	return S_OK;
}

_int CPlayerWeapon::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CPlayerWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	Key_Input();
	Mouse_Input();

	//애니메이션 실행
	m_pAnimatorCom->Play_Animation(m_eWeaponId + m_eWeaponState, 0, fTimeDelta);

	// 만약 현재 애니메이션이 다 끝난 상태라면 총의 상태를 기본 상태로 돌려놓는다.
	if (m_pAnimatorCom->Get_isFinAct())
		m_eWeaponState = IDLE;
}

void CPlayerWeapon::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_eWeaponId);
	
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
	m_pAnimatorCom->Add_Animation(PISTOL, m_pTextureCom, m_pBufferCom, 5, 1, 0, 0, 0.1f, true);
	m_pAnimatorCom->Add_Animation(PISTOL + SHOT, m_pTextureCom, m_pBufferCom, 5, 1, 0, 4, 0.1f, true);
	
	// Shotgun Animation
	m_pAnimatorCom->Add_Animation(SHOTGUN, m_pTextureCom, m_pBufferCom, 8, 1, 0, 0, 0.2f, true);
	m_pAnimatorCom->Add_Animation(SHOTGUN + SHOT, m_pTextureCom, m_pBufferCom, 8, 1, 0, 7, 0.1f, true);

	// Chaingun Animation
	m_pAnimatorCom->Add_Animation(CHAINGUN, m_pTextureCom, m_pBufferCom, 3, 1, 0, 0, 0.1f, true);
	m_pAnimatorCom->Add_Animation(CHAINGUN + SHOT, m_pTextureCom, m_pBufferCom, 3, 1, 1, 2, 0.1f, true);
}

HRESULT CPlayerWeapon::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectSpriteTex*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_RectSpriteTex"));
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
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_PlayerWeaponTexture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	//Animator 애니메이션 세팅
	pComponent = m_pAnimatorCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CPlayerWeapon::Key_Input()
{
	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_1) & 0x80)
	{
		// 총을 바꾸려 할 때 사격 상태라면 리턴
		if (m_eWeaponState == SHOT)
			return;

		m_eWeaponId = PISTOL;
		m_eWeaponState = IDLE;
	}
	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_2) & 0x80)
	{
		// 총을 바꾸려 할 때 사격 상태라면 리턴
		if (m_eWeaponState == SHOT)
			return;

		m_eWeaponId = SHOTGUN;
		m_eWeaponState = IDLE;
	}
	if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_3) & 0x80)
	{
		// 총을 바꾸려 할 때 사격 상태라면 리턴
		if (m_eWeaponState == SHOT)
			return;

		m_eWeaponId = CHAINGUN;
		m_eWeaponState = IDLE;
	}

}

void CPlayerWeapon::Mouse_Input()
{
	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
	{
		m_eWeaponState = SHOT;
	}
	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_RB) & 0x80)
	{
		m_eWeaponState = IDLE;
	}
}


CPlayerWeapon* CPlayerWeapon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerWeapon* pWeapon = new CPlayerWeapon(pGraphicDev);

	if (FAILED(pWeapon->Ready_GameObject()))
	{
		Safe_Release(pWeapon);
		MSG_BOX("Weapon Create Failed");
		return pWeapon;
	}

	return pWeapon;
}

void CPlayerWeapon::Free()
{
	Engine::CGameObject::Free();
}
