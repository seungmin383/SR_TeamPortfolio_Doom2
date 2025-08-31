#pragma once
#include "CBase.h"
#include"Engine_Define.h"
#include "CParticle.h"

BEGIN(Engine)

enum class  PARTICLEID
{
	BLOOD_EXPLOSION,			//피터지는 파티클
	BULLET_IMPACT,					//총알이 맞는 파티클
	END_PARTICLE
};

class ENGINE_DLL CParticleManager :public CBase
{
	DECLARE_SINGLETON(CParticleManager)

private:
    explicit CParticleManager();
    virtual ~CParticleManager();

public:
	virtual HRESULT Ready_Particle();
	virtual void Update_Particle(const _float& fTimeDelta);
	virtual void LateUpdate_Particle(const _float& fTimeDelta);
	virtual void Render_Particle(const _float& fTimeDelta);


	void Start_Particle(
		const _vec3& vPos, 
		const _vec3& vDir, 
		const _float& fSize, 
		const _float& fAngle,
		const _float& fLifeTime, 
		const _float& fSpeed,
		const _uint& iTextureIndex);


	void Add_Particle(CParticle* _Particle) { m_vecParticlePool.push_back(_Particle); }

private	:
	vector<CParticle*> m_vecParticlePool;				// 비활성화된 파티클들
	vector <CParticle*> m_vecActiveParticle;			// 활성화된 파티클들 -> 매 프레임마다 업데이트, 렌더링

private:
	virtual void Free();
};

END