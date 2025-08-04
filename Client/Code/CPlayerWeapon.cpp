#include "pch.h"
#include "CPlayerWeapon.h"
#include "CRenderer.h"

CPlayerWeapon::CPlayerWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CPlayerWeapon::CPlayerWeapon(const CGameObject& rhs)
	: CGameObject(rhs)
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
	
	vScale = { 100.f, 100.f, 1.f };
	vPos = { 0.f, -WINCY * 0.5f + vScale.y + 15.f , 1.f };
	
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

	//애니메이션 실행
	m_pAnimatorCom->Play_Animation(0, 0, fTimeDelta);
}

void CPlayerWeapon::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);
	
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
	m_pAnimatorCom->Add_Animation(0, m_pTextureCom, m_pBufferCom, 3, 1, 0, 2, 0.1f, true);
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
