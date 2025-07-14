#pragma once
#include "CBase.h"
#include "CFrame.h"

BEGIN(Engine)

class ENGINE_DLL CFrameManager : public CBase
{
	DECLARE_SINGLETON(CFrameManager)

private:
	explicit CFrameManager();
	virtual ~CFrameManager();

public:
	_bool	IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
	HRESULT Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);

private:
	CFrame* Find_Frame(const _tchar* pFrameTag);

private:
	map<const _tchar*, CFrame*>			m_mapFrame;

public:
	virtual void	Free();
};

END