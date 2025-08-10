#include "CAnimation.h"
#include "CVIBuffer.h"

CAnimation::CAnimation()
	: m_pBufferCom(nullptr),
	m_iStartFrame(0),
	m_iEndFrame(0),
	m_iCurFrame(0),
	m_iCurRow(-1),
	m_iPreRow(-1),
	m_fDelay(0.f),
	m_bIsLoop(true),
	m_bIsEnd(false),
	m_bFinAct(false),
	m_bIsReverse(false)
{
	ZeroMemory(&m_vUVPerColRow, sizeof(_vec2));
}

CAnimation::~CAnimation()
{
}

CAnimation* CAnimation::Create(CTexture* _pTextureCom,
	CVIBuffer* _pBufferCom,
	_uint _iTextureColumn,
	_uint _iTextureRow,
	_uint _iStartFrame,
	_uint _iEndFrame,
	_float _fDelay,
	_bool _bIsLoop,
	_bool _bIsReverse)
{
	CAnimation* pAnimation = new CAnimation();

	if (FAILED(pAnimation->Ready_Animation(_pTextureCom,
		_pBufferCom,
		_iTextureColumn,
		_iTextureRow,
		_iStartFrame,
		_iEndFrame,
		_fDelay,
		_bIsLoop,
		_bIsReverse)))
	{
		Safe_Release(pAnimation);
		MSG_BOX("Animation Create Failed");
		return nullptr;
	}

	return pAnimation;
}

HRESULT CAnimation::Ready_Animation(CTexture* _pTextureCom,
	CVIBuffer* _pBufferCom,
	_uint _iTextureColumn,
	_uint _iTextureRow,
	_uint _iStartFrame,
	_uint _iEndFrame,
	_float _fDelay,
	_bool _bIsLoop,
	_bool _bIsReverse)
{
	m_pTextureCom = _pTextureCom;
	m_pBufferCom = _pBufferCom;
	m_TextureColumn = _iTextureColumn;
	m_TextureRow = _iTextureRow;
	m_iStartFrame = _iStartFrame;
	m_iEndFrame = _iEndFrame;
	m_fDelay = _fDelay;
	m_bIsLoop = _bIsLoop;
	m_bIsReverse = _bIsReverse;

	m_vUVPerColRow.x = 1.f / _iTextureColumn;
	m_vUVPerColRow.y = 1.f / _iTextureRow;

	return S_OK;
}

void CAnimation::Play_Animation(_uint _irow, const _float& fTimeDelta)
{
	if (_irow != m_iCurRow)
	{
		SetFrameDefault();
		m_iPreRow = m_iCurRow;
		m_iCurRow = _irow;
	}

	if (m_bIsReverse == false)
		Calc_Frame(fTimeDelta);
	else
		Calc_Frame_Reverse(fTimeDelta);
}

void CAnimation::Set_Texture(_uint _setTexNum)
{
	m_pTextureCom->Set_Texture(_setTexNum);
}

void CAnimation::Set_UVCoord()
{
	m_pBufferCom->Set_UVCoord(m_iCurFrame, m_vUVPerColRow.x, m_iCurRow, m_vUVPerColRow.y, m_bIsReverse);
}

void CAnimation::Calc_Frame(const _float& fTimeDelta)
{
	if (m_bIsEnd)
		return;

	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > m_fDelay)
	{
		m_iCurFrame++;
		m_fFrameTime = 0.f;

		if (m_bFinAct)
			m_bFinAct = false;

		if (m_iCurFrame > (_int)m_iEndFrame)
		{
			m_bFinAct = true;

			if (m_bIsLoop)
				m_iCurFrame = (_int)m_iStartFrame;
			else
			{
				m_bIsEnd = true;
				return;
			}
		}

		Set_UVCoord();
	}
}

void CAnimation::Calc_Frame_Reverse(const _float& fTimeDelta)
{
	if (m_bIsEnd)
		return;

	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > m_fDelay)
	{
		m_iCurFrame--;
		m_fFrameTime = 0.f;

		if (m_bFinAct)
			m_bFinAct = false;

		if (m_iCurFrame < (_int)m_iStartFrame)
		{
			m_bFinAct = true;

			if (m_bIsLoop)
				m_iCurFrame = (_int)m_iEndFrame;
			else
				m_bIsEnd = true;
		}

		Set_UVCoord();
	}
}

void CAnimation::SetFrameDefault()
{
	if (m_bIsReverse == false)
		m_iCurFrame = (_uint)m_iStartFrame;
	else
		m_iCurFrame = (_uint)m_iEndFrame;

	m_fFrameTime = 0.f;
	m_bIsEnd = false;
	m_iCurRow = 0;
	m_bFinAct = false;
}

void CAnimation::Free()
{
}
