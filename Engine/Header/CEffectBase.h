#pragma once
#include "IEffect.h"
#include"CPrototypeManager.h"

BEGIN(Engine)

class ENGINE_DLL CEffectBase :public IEffect
{
	protected:
		explicit CEffectBase();
		virtual ~CEffectBase();

public:
		virtual void Ready_Effect(const _vec3& vPos) override;
		virtual _int Update_Effect(const _float& fTimeDelta) override;
		void LateUpdate_Effect() override;
		virtual void Render_Effect(const _float& fTimeDelta) override;
		virtual IEffect* Clone() override;
		virtual _vec3 Get_Pos() override;

protected:
	map<const _tchar*, CComponent*>         m_mapComponent[ID_END];

	LPDIRECT3DDEVICE9                       m_pGraphicDev;
	CRectSpriteTex* m_pBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CAnimator* m_pAnimatorCom = nullptr;
	
	private:
		virtual void Free() override;

};

END