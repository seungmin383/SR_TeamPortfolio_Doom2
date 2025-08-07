#include "CAnimator.h"
#include "CAnimation.h"

CAnimator::CAnimator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev),
	m_iCurState(-1),
	m_iPreState(-1),
	m_pCurAnimation(nullptr)
{
}

CAnimator::CAnimator(const CAnimator& rhs)
	: CComponent(rhs),
	m_iCurState(rhs.m_iCurState),
	m_iPreState(rhs.m_iPreState),
	m_pCurAnimation(rhs.m_pCurAnimation)
{
}

CAnimator::~CAnimator()
{
}

HRESULT CAnimator::Ready_Animator()
{
	return S_OK;
}

HRESULT CAnimator::Add_Animation(_uint _iCurState,
								CTexture* _pTextureCom,
								CVIBuffer* _pBufferCom,
								_uint _iTextureColumn,
								_uint _iTextureRow,
								_uint _iStartFrame,
								_uint _iEndFrame,
								_float _fDelay,
								_bool _bIsLoop,
								_bool _bIsReverse)
{
	CAnimation* pAnimation = CAnimation::Create(_pTextureCom,
									_pBufferCom,
									_iTextureColumn,
									_iTextureRow,
									_iStartFrame,
									_iEndFrame,
									_fDelay,
									_bIsLoop,
									_bIsReverse);

	m_mapAnimation.insert({ _iCurState, pAnimation });

	return S_OK;
}

void CAnimator::Play_Animation(_uint _iState, _uint _iRow, const _float& fTimeDelta, _uint _setTexNum)
{
	if (m_iCurState != _iState)
	{
		m_iPreState = m_iCurState;
		m_iCurState = _iState;

		auto iter = find_if(m_mapAnimation.begin(), m_mapAnimation.end(), [&_iState](pair<const _uint, CAnimation*> innerIter) {return innerIter.first == _iState; });
		
		if (iter == m_mapAnimation.end())
			m_pCurAnimation = nullptr;
		else
		{
			m_pCurAnimation = iter->second;
			m_pCurAnimation->SetFrameDefault();
			m_pCurAnimation->Set_UVCoord();
		}
	}

	if (m_pCurAnimation == nullptr)
		return;

	m_pCurAnimation->Set_Texture(_setTexNum);
	m_pCurAnimation->Play_Animation(_iRow, fTimeDelta);
}

CComponent* CAnimator::Clone()
{
	return new CAnimator(*this);
}

CAnimator* CAnimator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAnimator* pAnimator = new CAnimator(pGraphicDev);

	return pAnimator;
}

void CAnimator::Free()
{
	for_each(m_mapAnimation.begin(), m_mapAnimation.end(), CDeleteMap());

	CComponent::Free();
}
