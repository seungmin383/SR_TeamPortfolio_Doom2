#include "EffectManager.h"
#include "CPrototypeManager.h"
#include "CManagement.h"
#include <iostream>

IMPLEMENT_SINGLETON(EffectManager)

EffectManager::EffectManager()
{
	AllocConsole();	

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

}

EffectManager::~EffectManager()
{
	Free();
}

HRESULT EffectManager::Ready_Effect()
{
	for (int i = 0; i < static_cast<_uint>(EFFECTID::END_EFFECT); i++)
		m_vecEffectPool[i].reserve(100);


	return S_OK;
}

void EffectManager::Update_Effect(const _float& fTimeDelta)
{

	if (m_vecActiveEffect.size() == 0)
		return;


	for (auto iter = m_vecActiveEffect.begin(); iter != m_vecActiveEffect.end();)
	{
		_int iExit = (*iter)->Update_GameObject(fTimeDelta);				//이펙트 업데이트

		if (iExit == -1)																	//이펙트가 종료되었으면
		{
			m_vecEffectPool[static_cast<_int>((*iter)->Get_EffectId())].push_back(*iter);							//풀에 넣어주고
			iter = m_vecActiveEffect.erase(iter);							//활성화 벡터에서 지워주기
		}
		else
			++iter;
	}
	
}

void EffectManager::LateUpdate_Effect(const _float& fTimeDelta)
{
	if (m_vecActiveEffect.size() == 0)
		return;

	for (auto iter = m_vecActiveEffect.begin(); iter != m_vecActiveEffect.end();iter++)
		(*iter)->LateUpdate_GameObject(fTimeDelta);				//이펙트 업데이트

	return;
}


void EffectManager::Start_Effect(const _vec3& _vPos, EFFECTID _Id)
{
	_uint ID = static_cast<_uint>(_Id);

	if (m_vecEffectPool[ID].size() == 0)							//만약 풀에 이펙트가 없다면
	{
		CEffect* pInstance = Clone_Effect(_Id, _vPos);					//이펙트를 복제해서

		if (pInstance == nullptr)
			return;

		pInstance->Set_EffectId(_Id);
		pInstance->Reset_Effect(_vPos);																//초기화 해주고

		m_vecActiveEffect.push_back(pInstance);		//활성화 벡터에 넣어주고
		return;
	}

	//풀에 이펙트가 있다면
	CEffect* pInstance = m_vecEffectPool[ID].back();							//가장 뒤에있는 원소를 빼고

	pInstance->Set_EffectId(_Id);
	pInstance->Reset_Effect(_vPos);																//초기화 해주고

	m_vecActiveEffect.push_back(pInstance);										//활성화 벡터에 넣어주고
	m_vecEffectPool[ID].pop_back();														//원소 지워주기

	return;
}

void EffectManager::Add_Effect(EFFECTID _Id, CEffect* _Effect)
{
	CEffect* pEffect = Find_Effect(_Id);						//이펙트가 있는지 확인하고

	if (pEffect != nullptr)											//있다면 return
		return;

	m_mapEffectPrototype.insert({ _Id, _Effect });		//없으면 원본으로 저장
}

void EffectManager::Reset_EffectPool()
{
	for(int i = 0; i < static_cast<_uint>(EFFECTID::END_EFFECT); i++)
	{
		for (auto iter = m_vecEffectPool[i].begin(); iter != m_vecEffectPool[i].end();)
		{
			if ((*iter) != nullptr)
			{
				iter = m_vecEffectPool[i].erase(iter);
				*iter = nullptr;
			}
		}
		m_vecEffectPool[i].clear();
	}
}

CEffect* EffectManager::Clone_Effect(EFFECTID _Id, _vec3 vPos)
{
	CEffect* pEffect = Find_Effect(_Id);						//이펙트가 있는지 확인하고

	if (pEffect == nullptr)											//없다면 return
		return nullptr;

	CEffect* pCloneEffect = pEffect->Clone(vPos);			//있다면 복사

	return pCloneEffect;
}

CEffect* EffectManager::Find_Effect(EFFECTID _Id)
{

	auto iter = find_if(m_mapEffectPrototype.begin(), m_mapEffectPrototype.end(), [&_Id](const pair<EFFECTID, CEffect*> pair) { return pair.first == _Id; });

	if (iter == m_mapEffectPrototype.end())
		return nullptr;

	return iter->second;
}
void EffectManager::Free()
{
	// 1) Pool 비활성 이펙트 전부 해제
	for (auto& bucket : m_vecEffectPool)
	{
		for (auto* p : bucket)
		{
			if (p) Safe_Release(p);   // 또는 Safe_Delete(p);
		}
		bucket.clear();
		// 필요시: bucket.shrink_to_fit();
	}

	// 2) 활성 이펙트 전부 해제
	for (auto& pEffet : m_vecActiveEffect)
	{
		Safe_Release(pEffet);   // 또는 Safe_Delete(pEffet);
		// 필요시: bucket.shrink_to_fit();
	}

	// 3) 프로토타입 해제
	for (auto& kv : m_mapEffectPrototype)
	{
		if (kv.second) Safe_Release(kv.second); // 또는 Safe_Delete(kv.second);
	}
	m_mapEffectPrototype.clear();
}

//void EffectManager::Free()
//{
//	for (int i = 0; i < static_cast<_uint>(EFFECTID::END_EFFECT); i++)
//	{
//		for (auto iter = m_vecEffectPool[i].begin(); iter != m_vecEffectPool[i].end();)
//		{
//			if ((*iter) != nullptr)
//			{
//				iter = m_vecEffectPool[i].erase(iter);
//				*iter = nullptr;
//			}
//		}
//		m_vecEffectPool[i].clear();
//	}
//
//	for (int i = 0; i < static_cast<_uint>(EFFECTID::END_EFFECT); i++)
//	{
//		for (auto iter = m_vecActiveEffect[i].begin(); iter != m_vecActiveEffect[i].end();)
//		{
//			if ((*iter) != nullptr)
//			{
//				iter = m_vecActiveEffect[i].erase(iter);
//				*iter = nullptr;
//			}
//		}
//		m_vecActiveEffect[i].clear();
//	}
//
//	for_each(m_mapEffectPrototype.begin(), m_mapEffectPrototype.end(), CDeleteMap());
//		m_mapEffectPrototype.clear();
//
//}