#include "CEffect.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CEffect::CEffect(const CEffect& rhs)
	:CGameObject(rhs), m_eEffectId(), m_iFunc_Ent_Cnt(0)
{
}

CEffect::~CEffect()
{
}

HRESULT CEffect::Ready_GameObject()
{
	CGameObject::Ready_GameObject();

    return S_OK;
}

_int CEffect::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	m_bDead = Check_isAnim_Fin();


	return 0;
}

void CEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CEffect::Render_GameObject(const _float& fTimeDelta)
{
	CGameObject::Render_GameObject(fTimeDelta);
}

void CEffect::Reset_Effect(_vec3 vPos)
{
}

_bool CEffect::Check_isAnim_Fin()
{
	if (m_iFunc_Ent_Cnt == 0)
	{
		m_iFunc_Ent_Cnt++;
		return FALSE;
	}
	else if (m_pAnimCom->Get_isAnimEnd())
	{
		m_iFunc_Ent_Cnt = 0;
		m_bDead = TRUE;
		return TRUE;
	}

	m_iFunc_Ent_Cnt++;
	return FALSE;
}

void CEffect::Bill_Board()
{
	_matrix	matWorld, matView, matBill;

	//그 오브젝트에 월드 행렬 구해오기
	m_pTransformCom->Get_World(&matWorld);

	//뷰행렬을 가져오기
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	// 뷰 스페이스 -> 카메라 월드 행렬의 역행렬
	//뷰행렬에 y 회전만 가져오기
	//뷰행렬의 역행렬은 카메라의 월드 행렬
	//지금 y축 회전은 월드 행렬이고
	//나머지는 뷰스페이스 행렬
	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);		// 카메라가 갖고 있는 y회전의 역행렬 -역행렬> 카메라의 월드 상태의 y축 회전 행렬

	matWorld = matBill * matWorld;

	m_pTransformCom->Set_World(&matWorld);

}

CEffect* CEffect::Clone()
{
    return nullptr;
}

CEffect* CEffect::Clone(_vec3 vPos)
{
	return nullptr;
}

HRESULT CEffect::Add_Component()
{
	CComponent* pComponent = nullptr;

	//Buffer
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

	//Animator
	pComponent = m_pAnimCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Animator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CEffect::Free()
{
	CGameObject::Free();
}
