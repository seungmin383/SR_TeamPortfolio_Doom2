#pragma once
#include "CComponent.h"
#include "CVIBuffer.h"
#include "CTexture.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation :
    public CBase
{
private:
    explicit CAnimation();
    virtual ~CAnimation();

public:
    static CAnimation*              Create(CTexture* _pTextureCom,
                                           CVIBuffer* _pBufferCom,
                                           _uint _iTextureColumn,
                                           _uint _iTextureRow,
                                           _uint _iStartFrame,
                                           _uint _iEndFrame,
                                           _float _fDelay,
                                           _bool _bIsLoop = true,
                                           _bool _isReverse = false);

    HRESULT                         Ready_Animation(CTexture* _pTextureCom, 
                                                    CVIBuffer* _pBufferCom,
                                                    _uint _iTextureColumn,
                                                    _uint _iTextureRow,
                                                    _uint _iStartFrame,
                                                    _uint _iEndFrame,
                                                    _float _fDelay,
                                                    _bool _bIsLoop = true,
                                                    _bool _isReverse = false);

    void                            Play_Animation(_uint _irow, const _float& fTimeDelta);
    void                            Set_Texture(_uint _setTexNum = 0);
    void                            SetFrameDefault();
    _bool                           Get_isFinAct() { return m_bFinAct; }
	_bool 					           Get_isAnimEnd() { return m_bIsEnd; } 

	void                                Set_isAnimEnd(_bool _b) { m_bIsEnd = _b; } 
    void                            Set_UVCoord();

private:
    void                            Calc_Frame(const _float& fTimeDelta);
    void                            Calc_Frame_Reverse(const _float& fTimeDelta);

public:
    CVIBuffer*                      m_pBufferCom;
    CTexture*                       m_pTextureCom;

    _uint                           m_TextureColumn;
    _uint                           m_TextureRow;

    _uint                           m_iStartFrame;
    _uint                           m_iEndFrame;
    _int                            m_iCurFrame;

    _uint                           m_iCurRow;
    _uint                           m_iPreRow;

    _float                          m_fDelay;
    _float                          m_fFrameTime;
    _bool                           m_bIsLoop;
    _bool                           m_bIsEnd;
    _bool                           m_bIsReverse;

    _bool                           m_bFinAct;

    _vec2                           m_vUVPerColRow;

private:
    virtual void                    Free();
};

END