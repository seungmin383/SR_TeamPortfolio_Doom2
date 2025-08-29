#include "pch.h"
#include "CUIObject.h"

CUIObject::CUIObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_pBufferCom(nullptr),
	m_pTransformCom(nullptr),
	m_pTextureCom(nullptr),
	m_pCameraCom(nullptr)
{
	ZeroMemory(&rcUI, sizeof(RECT));
}

CUIObject::CUIObject(const CUIObject& rhs)
	: CGameObject(rhs),
	m_pBufferCom(rhs.m_pBufferCom),
	m_pTransformCom(rhs.m_pTransformCom),
	m_pTextureCom(rhs.m_pTextureCom),
	m_pCameraCom(rhs.m_pCameraCom)
{
	ZeroMemory(&rcUI, sizeof(RECT));
}

CUIObject::~CUIObject()
{
}

void CUIObject::Update_UI()
{
	rcUI = { (LONG)(m_fX - m_fSizeX * 0.5f), (LONG)(m_fY - m_fSizeY * 0.5f), (LONG)(m_fX + m_fSizeX * 0.5f), (LONG)(m_fY + m_fSizeY * 0.5f) };

	m_pTransformCom->Set_Scale(_vec3(m_fSizeX * 0.5f, m_fSizeY * 0.5f, 0.f));
	m_pTransformCom->Set_Pos(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f);
}

_bool CUIObject::Catch_Mouce()
{
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (true == PtInRect(&rcUI, ptMouse))
		return true;

	return false;
}