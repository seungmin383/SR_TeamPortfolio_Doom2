#pragma once
#include "CGameObject.h"
#include "CDynamicCamera.h"
#include "CPrototypeManager.h"
#include "CDoor.h"

class CSwitch_Button :    public CGameObject
{
private:
	explicit CSwitch_Button(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSwitch_Button(const CGameObject& rhs);
	virtual ~CSwitch_Button();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

	void Set_Target(CDoor* pDoor) { m_pObjTarget = pDoor; }

private:
	bool m_bPressed = false;

private:
	HRESULT Add_Component();
	void        Set_OnTerrain();
	void		Bill_Board();

private:
	Engine::CRectSpriteTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;						// 스위치 초기상태
	Engine::CTexture* m_pTextureCom2;						// 스위치 작동(대부분 켜져 있는걸 끄기)
	Engine::CCalculator* m_pCalculatorCom;

	CDoor* m_pObjTarget;

private:
	enum class State { Idle, Dead };
	State           m_state = State::Idle;

private:
	void SetSwitchOn();

public:
	static CSwitch_Button* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CSwitch_Button* Clone() override;

private:
	virtual void Free();

};

