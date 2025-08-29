#include "CUICamera.h"
#include "CRenderer.h"

CUICamera::CUICamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIObject(pGraphicDev)
{
}

CUICamera::CUICamera(const CUICamera& rhs)
	: CUIObject(rhs)
{
}

CUICamera::~CUICamera()
{
}

HRESULT CUICamera::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	
	D3DXMatrixIdentity(&m_matWorld);

	m_pCameraCom->Ready_Camera(true);

	return S_OK;
}

_int CUICamera::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	m_pCameraCom->Set_Projection(&m_matWorld);

	return 0;
}

void CUICamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CUICamera::Render_GameObject(const _float& fTimeDelta)
{
}

HRESULT CUICamera::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pCameraCom = dynamic_cast<Engine::CCameraCom*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_OrthoCamera"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CUICamera* CUICamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUICamera* pUICamera = new CUICamera(pGraphicDev);

	if (FAILED(pUICamera->Ready_GameObject()))
	{
		Safe_Release(pUICamera);
		MSG_BOX("UICamera Create Failed");
		return nullptr;
	}

	return pUICamera;
}

CGameObject* CUICamera::Clone()
{
	return new CUICamera(*this);
}

void CUICamera::Free()
{
	Engine::CGameObject::Free();
}
