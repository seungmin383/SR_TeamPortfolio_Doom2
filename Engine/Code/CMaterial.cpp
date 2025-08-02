#include "CMaterial.h"

CMaterial::CMaterial(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
	ZeroMemory(&m_tMtrlInfo, sizeof(D3DMATERIAL9));
}

CMaterial::CMaterial(const CMaterial& rhs)
	: CComponent(rhs),
	m_tMtrlInfo(rhs.m_tMtrlInfo)
{
}

CMaterial::~CMaterial()
{
}

HRESULT CMaterial::Ready_Material(const D3DMATERIAL9* pMtrlInfo)
{
	memcpy(&m_tMtrlInfo, pMtrlInfo, sizeof(D3DMATERIAL9));
	
	return S_OK;
}

HRESULT CMaterial::Set_Material()
{
	m_pGraphicDev->SetMaterial(&m_tMtrlInfo);

	return S_OK;
}

CComponent* CMaterial::Clone()
{
	return new CMaterial(*this);
}

CMaterial* CMaterial::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMaterial* pMaterial = new CMaterial(pGraphicDev);

	return pMaterial;
}

void CMaterial::Free()
{
	CComponent::Free();
}
