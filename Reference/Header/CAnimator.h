#pragma once
#include "CComponent.h"
#include "CVIBuffer.h"
#include "CTexture.h"
#include "CAnimation.h"

BEGIN(Engine)

class CAnimation;

/// <summary>
/// 상태, 스프라이트 애니메이션을 맵 컨테이너 타입으로 관리한다
/// </summary>
class ENGINE_DLL CAnimator :
    public CComponent
{
    explicit CAnimator(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CAnimator(const CAnimator& rhs);
    virtual ~CAnimator();

public:
    /// <summary>
    /// Empty Function
    /// </summary>
    /// <returns></returns>
    HRESULT                 Ready_Animator();

    /// <summary>
    /// 애니메이션 추가
    /// </summary>
    /// <param name="_iCurState">추가할 상태값을 넣어준다 enum으로 관리 시 _uint로 캐스팅한다</param>
    /// <param name="_pTextureCom">애니메이션 스프라이트를 가진 텍스처 컴포넌트</param>
    /// <param name="_pBufferCom">애니메이션을 지정할 버텍스 버퍼</param>
    /// <param name="_iTextureColumn">스프라이트 세로 개수</param>
    /// <param name="_iTextureRow">스프라이트 가로 개수</param>
    /// <param name="_iStartFrame">시작 프레임</param>
    /// <param name="_iEndFrame">끝 프레임</param>
    /// <param name="_fDelay">딜레이가 클 수록 애니메이션이 느리다</param>
    /// <param name="_bIsLoop">반복할거냐</param>
    /// <returns></returns>
    HRESULT                 Add_Animation(_uint _iCurState,
                                        CTexture* _pTextureCom,
                                        CVIBuffer* _pBufferCom,
                                        _uint _iTextureColumn,
                                        _uint _iTextureRow,
                                        _uint _iStartFrame,
                                        _uint _iEndFrame,
                                        _float _fDelay,
                                        _bool _bIsLoop = true,
                                        _bool _bIsReverse = false);

    /// <summary>
    /// 상태에 맞는 애니메이션을 실행
    /// </summary>
    /// <param name="_iState">상태</param>
    /// <param name="_iRow">실행할 스프라이트 가로 번호</param>
    /// <param name="fTimeDelta"></param>
    void                    Play_Animation(_uint _iState, _uint _iRow, const _float& fTimeDelta, _uint _setTexNum = 0);

    _bool                   Get_isFinAct()
    { 
        return m_pCurAnimation == nullptr ? false :  m_pCurAnimation->Get_isFinAct();
    }

public:
    virtual CComponent*     Clone();
    static CAnimator*       Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    map<_uint, CAnimation*> m_mapAnimation;
    _uint                   m_iCurState;
    _uint                   m_iPreState;
    CAnimation*             m_pCurAnimation;

private:
    virtual void            Free();
};

END