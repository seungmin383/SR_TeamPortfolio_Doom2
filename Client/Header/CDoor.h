#pragma once
#include "CGameObject.h"
#include "CDynamicCamera.h"
#include "CPrototypeManager.h"

class CDoor :    public CGameObject
{
private:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDoor(const CGameObject& rhs);
	virtual ~CDoor();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

	void OpenDoor();

// 문이랑 스위치 태그기능
public:
	void Set_LinkId(const wstring& id) { m_linkId = id; }
	const wstring& Get_LinkId() const { return m_linkId; }

private:
	wstring m_linkId;

private:
	bool m_bPressed = false;
	_float m_fTimer  = 0.f;

private:
	HRESULT Add_Component();
	void        Set_OnTerrain();
	void		Bill_Board();
	void		DoorOpening(const _float& fTimeDelta);

private:
	enum class State { Idle, Opening, Open, Closing, Dead };
	State           m_state = State::Idle;
	_vec3 m_vOriginPos;
	_float m_fOpenHeight = 0.f;				// 문 올라가는 높이
	_float m_fStayOpenTime = 5.f;			// 문이 얼마나 열려있는가를 나타냄

private:
	Engine::CCubeTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;						// 스위치 초기상태
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CDoor* Clone() override;

private:
	virtual void Free();

};

