#include "CEffectBase.h"

CEffectBase::CEffectBase()
	:m_pGraphicDev(m_pGraphicDev)
{
	m_pGraphicDev->AddRef();	
}

CEffectBase::~CEffectBase()
{
}

void CEffectBase::Ready_Effect(const _vec3& vPos)
{
}

_int CEffectBase::Update_Effect(const _float& fTimeDelta)
{
	for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
		pComponent.second->Update_Component(fTimeDelta);

	return 0;
}

void CEffectBase::LateUpdate_Effect()
{
	for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
		pComponent.second->LateUpdate_Component();

	return;
}

void CEffectBase::Render_Effect(const _float& fTimeDelta)
{

}

IEffect* CEffectBase::Clone()
{
	return nullptr;
}

_vec3 CEffectBase::Get_Pos()
{
	_vec3 vPos;												//내 위치
	m_pTransformCom->Get_Pos(&vPos);	//내 위치 받아오기

	D3DXMATRIX matView;							//뷰 행렬


}

void CEffectBase::Free()
{
	for (_uint i = 0; i < ID_END; i++)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}

	Safe_Release(m_pGraphicDev);
}
