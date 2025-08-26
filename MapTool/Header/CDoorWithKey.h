#pragma once
#include "CGameObject.h"
#include "CDynamicCamera.h"
#include "CPrototypeManager.h"

class CDoorWithKey :    public CGameObject
{
private:
	explicit CDoorWithKey(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDoorWithKey(const CGameObject& rhs);
	virtual ~CDoorWithKey();

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
	_float m_fTimer = 0.f;

private:
	HRESULT Add_Component();
	void        Set_OnTerrain();
	void		Bill_Board();
	void		DoorOpening(const _float& fTimeDelta);

private:
	enum class State { Idle, Opening, Open, Closing, Dead };
	State           m_state = State::Idle;

	_vec3 m_vOriginPos;
	_float m_fOpenHeight = 4.f;
	_float m_fStayOpenTime = 5.f;

private:
	Engine::CRectSpriteTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;						
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CDoorWithKey* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CDoorWithKey* Clone() override;

private:
	virtual void Free();
};

