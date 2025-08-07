#include "pch.h"
#include "CGameUI_Info.h"
#include "CManagement.h"
#include "CRenderer.h"

CGameUI_Info::CGameUI_Info(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_pCamera(nullptr)
{
}

CGameUI_Info::CGameUI_Info(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

CGameUI_Info::~CGameUI_Info()
{
}

HRESULT CGameUI_Info::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

    return S_OK;
}

_int CGameUI_Info::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PLAYERUI, this);

    return _int();
}

void CGameUI_Info::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CGameUI_Info::Render_GameObject(const _float& fTimeDelta)
{
	if (m_pCamera == nullptr)
		return;

	_tchar szHp[128] = L"";
	_tchar szArmor[128] = L"";
	//_tchar szClip[128] = L"";

	swprintf_s(szHp, L"%d", m_pCamera->Get_Hp());
	swprintf_s(szArmor, L"%d", m_pCamera->Get_Armor());
	//swprintf_s(szClip, L"%d", m_pCamera->Get_Clip());

    _vec2 vPos1{ 380.f, 800.f };
	_vec2 vPos2{ 1000.f, 800.f };
	//_vec2 vPos3{ 1470.f, 776.f };

    CFontManager::GetInstance()->Render_Font(L"Font_Default", szHp, &vPos1, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	CFontManager::GetInstance()->Render_Font(L"Font_Default", szArmor, &vPos2, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//CFontManager::GetInstance()->Render_Font(L"Font_Default", szClip, &vPos3, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

void CGameUI_Info::OnCollision(CCollider* _pOther)
{
}

void CGameUI_Info::OnCollisionEnter(CCollider* _pOther)
{
}

void CGameUI_Info::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CGameUI_Info::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectTex*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_RectTex_UI"));
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
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_FaceUI"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	/*pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
	(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Collider"));
if (nullptr == pComponent)
	return E_FAIL;

m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });*/

	return S_OK;
}

CGameUI_Info* CGameUI_Info::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGameUI_Info* pInfo = new CGameUI_Info(pGraphicDev);

    if (FAILED(pInfo->Ready_GameObject()))
    {
        Safe_Release(pInfo);
        MSG_BOX("Player UI Info Create Failed");
        return nullptr;
    }

    return pInfo;
}

void CGameUI_Info::Free()
{
    Engine::CGameObject::Free();
}
