#pragma once
#include "CBase.h"
#include"Engine_Define.h"
#include "CPrototypeManager.h"
#include "IEffect.h"
#include "CParticle.h"

BEGIN(Engine)

enum class  EFFECTID
{
	BLOOD_EXPLOSION = 0,
	BULLET_IMPACT,
	TANPII_DROP,
	END_EFFECT
};

class ENGINE_DLL CEffetcManager :public CBase
{
	DECLARE_SINGLETON(CEffetcManager)

private:
    explicit CEffetcManager();
    virtual ~CEffetcManager();

public:
	virtual HRESULT Ready_Particle();
	virtual void Update_Particle(const _float& fTimeDelta);
	virtual void LateUpdate_Particle(const _float& fTimeDelta);
	virtual void Render_Particle(const _float& fTimeDelta);


	void Play_Effetc(
		const _vec3& vPos, 
		const _vec3& vDir, 
		const _float& fSize, 
		const _float& fAngle,
		const _float& fLifeTime, 
		const _float& fSpeed,
		const _uint& iTextureIndex);

private	:
	vector<IEffect*> m_vecEffectPool;				// 비활성화된 파티클들
	vector <IEffect*> m_vecActiveEffect;			// 활성화된 파티클들 -> 매 프레임마다 업데이트, 렌더링


	LPDIRECT3DDEVICE9                       m_pGraphicDev;

	CTexture*		m_pTexture[static_cast<_uint>(EFFECTID::END_EFFECT)] = { nullptr };
	CAnimation* m_pAnimation[static_cast<_uint>(EFFECTID::END_EFFECT)] = { nullptr };
	CRectSpriteTex* m_pBufferCom = nullptr;
	CTransform* m_pTransformCom = nullptr;

private:
	virtual void Free();
};

END