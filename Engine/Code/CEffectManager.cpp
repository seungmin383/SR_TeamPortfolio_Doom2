#include "CEffectManager.h"
#include "CPrototypeManager.h"

IMPLEMENT_SINGLETON(CEffectManager)

CEffectManager::CEffectManager()
	:m_pGraphicDev(m_pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

CEffectManager::~CEffectManager()
{
	Free();
}

HRESULT CEffectManager::Ready_Effect()
{
	//m_vecEffectPool.reserve(100);


	return S_OK;
}

void CEffectManager::Update_Effect(const _float& fTimeDelta)
{
	
}

void CEffectManager::LateUpdate_Effect(const _float& fTimeDelta)
{
}

void CEffectManager::Render_Effect(const _float& fTimeDelta)
{
}

void CEffectManager::Start_Effect(const _vec3& _vPos, EFFECTID _Id, IEffect* _Strategy)
{
	_uint ID = static_cast<_uint>(_Id);
	if (m_vecEffectPool[ID].size() == 0)							//만약 풀에 이펙트가 없다면
	{
		IEffect* pInstance = Clone_Effect(_Id);					//이펙트를 복제해서

		pInstance->Ready_Effect(_vPos);							//레디 부르고

		if (pInstance == nullptr)
			return;

		m_vecActiveEffect[ID].push_back(pInstance);		//활성화 벡터에 넣어주고
		return;
	}
	
	//풀에 이펙트가 있다면
	IEffect* pInstance = m_vecEffectPool[ID].back();		//가장 뒤에있는 원소를 빼고

	pInstance->Ready_Effect(_vPos);								//레디 부르고

	m_vecActiveEffect[ID].push_back(pInstance);			//활성화 벡터에 넣어주고
	m_vecEffectPool[ID].pop_back();								//원소 지워주기

	return;
}

void CEffectManager::Add_Effect(EFFECTID _Id, IEffect* _Effect)
{
	IEffect* pEffect = Find_Effect(_Id);						//이펙트가 있는지 확인하고

	if (pEffect != nullptr)											//있다면 return
		return;

	m_mapEffectPrototype.insert({ _Id, _Effect });		//없으면 원본으로 저장
}

IEffect* CEffectManager::Clone_Effect(EFFECTID _Id)
{
	IEffect* pEffect = Find_Effect(_Id);						//이펙트가 있는지 확인하고

	if (pEffect == nullptr)											//없다면 return
		return nullptr;

	IEffect* pCloneEffect = pEffect->Clone();			//있다면 복사

	return pCloneEffect;
}


IEffect* CEffectManager::Find_Effect(EFFECTID _Id)
{
	auto iter = find_if(m_mapEffectPrototype.begin(), m_mapEffectPrototype.end(), [&_Id](const pair<EFFECTID, IEffect*> pair) { return pair.first == _Id; });

	if (iter == m_mapEffectPrototype.end())
		return nullptr;

	return iter->second;
}

void CEffectManager::Free()
{
	for(int i = 0; i < static_cast<_uint>(EFFECTID::END_EFFECT); i++)
	{
		for(auto iter = m_vecEffectPool[i].begin(); iter != m_vecEffectPool[i].end();)
		{
			if((*iter) != nullptr)
			{
				iter = m_vecEffectPool[i].erase(iter);
				*iter = nullptr;
			}
		}
		m_vecEffectPool[i].clear();
	}

	Safe_Release(m_pGraphicDev);
}