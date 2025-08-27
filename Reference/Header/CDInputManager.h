#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CDInputManager :
    public CBase
{
    DECLARE_SINGLETON(CDInputManager)

private:
    explicit CDInputManager(void);
    virtual ~CDInputManager(void);

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tMouseState.rgbButtons[eMouse];
	}

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

	_bool	Get_KeyDown(_ubyte dwKey)
	{
		return (m_byKeyState[dwKey] & 0x80) && !(m_byOldKeyState[dwKey] & 0x80);	
	}

	_bool	Get_KeyUp(_ubyte dwKey)
	{
		return (!m_byKeyState[dwKey] & 0x80) && (m_byOldKeyState[dwKey] & 0x80);
	}

	_bool	Get_KeyHold(_ubyte dwKey)
	{
		return (m_byKeyState[dwKey] & 0x80);
	}

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_byOldKeyState[256];
	DIMOUSESTATE			m_tMouseState;

public:
	virtual void	Free(void);
};

END