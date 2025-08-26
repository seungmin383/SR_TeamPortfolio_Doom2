#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"

class CGameUI_Face : public CGameObject
{
private:
	explicit CGameUI_Face(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameUI_Face(const CGameObject& rhs);
	virtual ~CGameUI_Face();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

	void Set_EvilFace(const _float& fTime);

private:
	HRESULT Add_Component();

private:
	_float m_lFrame;

	CDynamicCamera* m_pCamera;

	// 얼굴 상태 변경을 위한 변수
	_bool m_bIsEvil;
	_float m_fEvilTime;

private:
	Engine::CRectTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;				// 체력 80퍼 이상
	Engine::CTexture* m_pTextureCom1;				// 체력 80퍼 미만
	Engine::CTexture* m_pTextureCom2;				// 체력 60퍼 미만
	Engine::CTexture* m_pTextureCom3;				// 체력 40퍼 미만
	Engine::CTexture* m_pTextureCom4;				// 체력 20퍼 미만
	Engine::CTexture* m_pTextureCom5;				// 무기 획득
	Engine::CTexture* m_pTextureCom6;				// 죽음

	Engine::CCalculator* m_pCalculatorCom;

public:
	static CGameUI_Face* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameUI_Face* Clone() override { return this; }

private:
	virtual void Free();
};