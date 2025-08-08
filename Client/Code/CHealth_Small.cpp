#include "pch.h"
#include "CHealth_Small.h"
#include "CRenderer.h"

CHealth_Small::CHealth_Small(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_iHeal(0), m_iFrame(0)
{
    CBase::Add_Ref();
}

CHealth_Small::CHealth_Small(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

CHealth_Small::~CHealth_Small()
{
}

HRESULT CHealth_Small::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_iHeal = 1;

	m_iFrame = 0;

	// 원본 텍스쳐의 크기와 비율을 정의
	const _float fTexWidth = 224.f;
	const _float fTexHeight = 32.f;
	const _float fTexAspectRatio = fTexWidth / fTexHeight;

	_vec3 vPos, vScale;

	// UI의 너비를 화면의 너비로 설정
	vScale.x = 30.f;
	vScale.y = vScale.x / fTexAspectRatio;		// 화면 너비를 10으로 나눈 값
	vScale.z = 1.f;

	// UI의 위치를 화면 맨 아래로 설정
	// UI의 중심점은 화면 하단 + UI 높이의 절반 위치에 둔다.
	vPos = { 150.f, -WINCY * 0.5f + vScale.y + 15.f, 0.f };

	// 계산된 위치와 스케일을 Transform 컴포넌트에 적용
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}

_int CHealth_Small::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	m_iFrame++;
	if (m_iFrame >= 4)
		m_iFrame = 0;

    return 0;
}

void CHealth_Small::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CHealth_Small::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iFrame);

	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CHealth_Small::OnCollision(CCollider* _pOther)
{
}

void CHealth_Small::OnCollisionEnter(CCollider* _pOther)
{
}

void CHealth_Small::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CHealth_Small::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Heal_Small"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

CHealth_Small* CHealth_Small::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHealth_Small* pHealthSmall = new CHealth_Small(pGraphicDev);

	if (FAILED(pHealthSmall->Ready_GameObject()))
	{
		Safe_Release(pHealthSmall);
		MSG_BOX("Health Small Create Failed");
		return nullptr;
	}

	return pHealthSmall;
}

void CHealth_Small::Free()
{
	//Engine::Safe_Delete(*this);

    Engine::CGameObject::Free();
}
