#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CTexture :
    public CComponent
{
protected:
    explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CTexture(const CTexture& rhs);
    virtual ~CTexture();

public:
    HRESULT Ready_Texture(const _tchar* pPath, TEXTUREID eType, const _uint& iCnt);
    void    Set_Texture(const _uint& iIndex = 0);

private:
    vector<IDirect3DBaseTexture9*>      m_vecTexture;

public:
    // 첫 번째 텍스처 핸들 반환 (여러 장일 경우 인덱스 사용 가능)
    IDirect3DBaseTexture9* GetTextureHandle(_uint idx = 0) const {
        if (m_vecTexture.empty() || idx >= m_vecTexture.size())
            return nullptr;
        return m_vecTexture[idx];
    }

public:
    virtual CComponent* Clone();
    static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        const _tchar* pPath,
        TEXTUREID eType,
        const _uint& iCnt = 1);

private:
    virtual void    Free();
};

END