#include "CEffetcManager.h"
IMPLEMENT_SINGLETON(CEffetcManager)

CEffetcManager::CEffetcManager()
{
}

CEffetcManager::~CEffetcManager()
{
	Free();
}

HRESULT CEffetcManager::Ready_Particle()
{
	m_vecParticlePool.reserve(100);


	return S_OK;
}

void CEffetcManager::Update_Particle(const _float& fTimeDelta)
{
}

void CEffetcManager::LateUpdate_Particle(const _float& fTimeDelta)
{
}

void CEffetcManager::Render_Particle(const _float& fTimeDelta)
{
}

void CEffetcManager::Free()
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