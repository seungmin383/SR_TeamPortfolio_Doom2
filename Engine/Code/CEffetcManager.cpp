#include "CParticleManager.h"
IMPLEMENT_SINGLETON(CParticleManager)

CParticleManager::CParticleManager()
{
}

CParticleManager::~CParticleManager()
{
	Free();
}

HRESULT CParticleManager::Ready_Particle()
{
	m_vecParticlePool.reserve(100);


	return S_OK;
}

void CParticleManager::Update_Particle(const _float& fTimeDelta)
{
}

void CParticleManager::LateUpdate_Particle(const _float& fTimeDelta)
{
}

void CParticleManager::Render_Particle(const _float& fTimeDelta)
{
}

void CParticleManager::Free()
{
	for(auto iter = m_vecParticlePool.begin(); iter != m_vecParticlePool.end();)
	{
		if((*iter) != nullptr)
		{
			iter = m_vecParticlePool.erase(iter);
			*iter = nullptr;
		}
	}

	m_vecParticlePool.clear();
}