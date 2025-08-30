#pragma once
#include "CBase.h"
#include"Engine_Define.h"
#include "IEffect.h"
#include "CParticle.h"

BEGIN(Engine)
class CPrototypeManager;

enum class  EFFECTID
{
	BLOOD_EXPLOSION = 0,
	BULLET_IMPACT,
	TANPII_DROP,
	END_EFFECT
};

class ENGINE_DLL CEffectManager :public CBase
{
	DECLARE_SINGLETON(CEffectManager)

private:
    explicit CEffectManager();
    virtual ~CEffectManager();

public:
	virtual HRESULT Ready_Effect();													
	virtual void Update_Effect(const _float& fTimeDelta);
	virtual void LateUpdate_Effect(const _float& fTimeDelta);
	virtual void Render_Effect(const _float& fTimeDelta);

public:
	// _vPos : 이펙트를 띄우고 싶은 위치, _Id : 이펙트 아이디
	void Start_Effect(const _vec3& _vPos, EFFECTID _Id);		//이펙트를 띄우고 싶은 곳에서 부르기 

public:
	virtual void Add_Effect(EFFECTID _Id, IEffect* _Effect);		//클라이언트에서 레디할 때 원본 생성해주기

private:
	virtual IEffect* Clone_Effect(EFFECTID _Id);							
	virtual IEffect* Find_Effect(EFFECTID _Id);			

private	:
	map <EFFECTID, IEffect*> m_mapEffectPrototype;	// 파티클 프로토타입들

	vector<IEffect*> m_vecEffectPool[static_cast<_uint>(EFFECTID::END_EFFECT)];				// 비활성화된 이펙트
	vector <IEffect*> m_vecActiveEffect[static_cast<_uint>(EFFECTID::END_EFFECT)];			// 활성화된 이펙트들 -> 매 프레임마다 업데이트, 렌더링


	LPDIRECT3DDEVICE9                       m_pGraphicDev;

	CTexture*		m_pTexture[static_cast<_uint>(EFFECTID::END_EFFECT)] = { nullptr };
	CAnimation* m_pAnimation[static_cast<_uint>(EFFECTID::END_EFFECT)] = { nullptr };
	CRectSpriteTex* m_pBufferCom = nullptr;
	CTransform* m_pTransformCom = nullptr;

private:
	virtual void Free();
};

END