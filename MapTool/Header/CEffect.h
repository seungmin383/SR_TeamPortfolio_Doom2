#pragma once
#include "CGameObject.h"
#include "Engine_Define.h"
#include"CPrototypeManager.h"
#include "CRenderer.h"

BEGIN(Engine)
class ENGINE_DLL CEffect : public CGameObject
{
protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect(const CGameObject& rhs);
	virtual ~CEffect();

public:
	virtual HRESULT         Ready_GameObject();
	virtual _int            Update_GameObject(const _float& fTimeDelta);
	virtual void            LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void            Render_GameObject(const _float& fTimeDelta);

	virtual void Reset_Effect();
	virtual _bool Check_isAnim_Fin();
public:
	virtual CEffect* Clone();
	virtual CEffect* Clone(_vec3 vPos);
	virtual HRESULT     Add_Component();

public:
	EFFECTID Get_EffectId() const { return m_eEffectId; }
	void Set_EffectId(EFFECTID eId) { m_eEffectId = eId; }

protected:
	_vec3 m_vPos = {};
	CRectSpriteTex* m_pBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CAnimator* m_pAnimCom = nullptr;

	EFFECTID m_eEffectId = EFFECTID::END_EFFECT;

protected:
	void Free() override;
};
END
