#pragma once
#include "CBase.h"
#include"Engine_Define.h"
#include "CEffect.h"

BEGIN(Engine)

class ENGINE_DLL EffectManager :public CBase
{
	DECLARE_SINGLETON(EffectManager)
private:
	explicit EffectManager();
	virtual ~EffectManager();

public:
	virtual HRESULT Ready_Effect();
	virtual void Update_Effect(const _float& fTimeDelta);
	virtual void LateUpdate_Effect(const _float& fTimeDelta);

public:
	// _vPos : 이펙트를 띄우고 싶은 위치, _Id : 이펙트 아이디
	void Start_Effect(const _vec3& _vPos, EFFECTID _Id);		//이펙트를 띄우고 싶은 곳에서 부르기 

public:
	virtual void Add_Effect(EFFECTID _Id, CEffect* _Effect);		//클라이언트에서 레디할 때 원본 생성해주기
	virtual void Reset_EffectPool();							//이펙트 풀 비우기

private:
	virtual CEffect* Clone_Effect(EFFECTID _Id, _vec3 vPos);
	virtual CEffect* Find_Effect(EFFECTID _Id);

private:
	map <EFFECTID, CEffect*> m_mapEffectPrototype;	// 파티클 프로토타입들

private:
	vector<CEffect*> m_vecEffectPool[static_cast<_uint>(EFFECTID::END_EFFECT)];				// 비활성화된 이펙트
	vector <CEffect*> m_vecActiveEffect;			// 활성화된 이펙트들 -> 매 프레임마다 업데이트, 렌더링

private:
	virtual void Free();
};
END