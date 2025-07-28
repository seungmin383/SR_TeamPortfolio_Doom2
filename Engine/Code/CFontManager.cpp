#include "CFontManager.h"

IMPLEMENT_SINGLETON(CFontManager)

CFontManager::CFontManager()
{
}

CFontManager::~CFontManager()
{
	Free();
}

HRESULT CFontManager::Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontTag, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	CFont* pMyFont = Find_Font(pFontTag);

	if (nullptr != pMyFont)
		return E_FAIL;

	pMyFont = CFont::Create(pGraphicDev, pFontType, iWidth, iHeight, iWeight);

	if (nullptr == pMyFont)
		return E_FAIL;

	m_mapFont.insert({ pFontTag, pMyFont });

	return S_OK;
}

void CFontManager::Render_Font(const _tchar* pFontTag, const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	CFont* pFont = Find_Font(pFontTag);

	pFont->Render_Font(pString, pPos, Color);
}

CFont* CFontManager::Find_Font(const _tchar* pFontTag)
{
	auto iter = find_if(m_mapFont.begin(), m_mapFont.end(), CTag_Finder(pFontTag));

	if (iter == m_mapFont.end())
		return nullptr;

	return iter->second;
}

void CFontManager::Free()
{
	for_each(m_mapFont.begin(), m_mapFont.end(), CDeleteMap());
	m_mapFont.clear();
}
