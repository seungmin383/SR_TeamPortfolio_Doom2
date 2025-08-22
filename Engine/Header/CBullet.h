#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class ENGINE_DLL CBullet : public CGameObject
{
protected:
	explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBullet(const CBullet& rhs);
	virtual ~CBullet();

public:
	virtual     HRESULT         Ready_GameObject()override;
	virtual     _int                  Update_GameObject(const _float& fTimeDelta)override;
	virtual     void                 LateUpdate_GameObject(const _float& fTimeDelta)override;
	virtual     void                 Render_GameObject(const _float& fTimeDelta)override;

	virtual void Linear_Movement(const _float& fTimeDelta);
	virtual void LockOn_Movement(const _float& fTimeDelta, _vec3 _TargetPos);

	virtual void Bill_Board();

public:
	virtual void			OnCollision(CCollider* _pOther)			override;
	virtual void			OnCollisionEnter(CCollider* _pOther)	override;
	virtual void			OnCollisionExit(CCollider* _pOther)	override;

public:
	CGameObject* Get_Owner() { return m_pOwner; };
	const _vec3 Get_InitDir() const { return m_vInitDir; };
	const _uint Get_Dmg() const { return m_iDmg; };

	void Set_Owner(CGameObject* _G) { m_pOwner = _G; };
	void Set_InitDir(_vec3 _V)  { m_vInitDir = _V; };
	void Set_Dmg(_uint _D)  {  m_iDmg = _D; };

protected:
	Engine::CRectSpriteTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CAnimator* m_pAnimCom = nullptr;

	//내 생각에 총알이 알고 있어야 될 것
	CGameObject* m_pOwner = nullptr;				//총알의 주인
	_vec3 m_vInitDir;												//발사할 때 총알의 방향
	//_float m_fSpeed;											//발사속도(gameobject에 있음)
	_uint m_iDmg = 0;											//총알 데미지

public:
	virtual HRESULT     Add_Component();

protected:
    virtual void        Free()override;
};

