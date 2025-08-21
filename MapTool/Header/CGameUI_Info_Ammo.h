#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CFontManager.h"
#include "CDynamicCamera.h"
#include "CPlayerWeapon.h"

class CGameUI_Info_Ammo : public CGameObject
{
private:
	explicit CGameUI_Info_Ammo(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameUI_Info_Ammo(const CGameObject& rhs);
	virtual ~CGameUI_Info_Ammo();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	HRESULT Add_Component();

public:
	void Set_Camera(CDynamicCamera* pCamera) { m_pCamera = pCamera; }

private:
	CDynamicCamera* m_pCamera;

private:
	Engine::CRectTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

private:
	vector<Engine::CTexture*> m_vecDigitTextures;

	_int m_iNum;
	_int m_hundreds;
	_int m_tens;
	_int m_ones;

	void Render_Number(int number, float baseX, float baseY, float scaleX, float scaleY);

public:
	static CGameUI_Info_Ammo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameUI_Info_Ammo* Clone() override { return this; }

private:
	virtual void Free();
};

