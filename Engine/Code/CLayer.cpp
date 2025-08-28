#include "CLayer.h"
#include "CPrototypeManager.h"
#include "CManagement.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent* CLayer::Get_Component(COMPONENTID eID, OBJID eObjId, const _tchar* pComponentTag)
{
	if (m_vecObject[(UINT)eObjId].empty())
		return nullptr;

	//해당 오브젝트 그룹의 첫 번째 요소를 가져옴
	auto iter = m_vecObject[(UINT)eObjId].begin();

	//TODO: 2025-08-03 다수의 오브젝트를 관리하는 상황에서 특정 오브젝트 찾는 로직 필요함
	// 현재는 플레이어 오브젝트를 찾는 용도로만 사용하는걸 가정하고있음
	
	return (*iter)->Get_Component(eID, pComponentTag);
}

HRESULT CLayer::Add_GameObject(OBJID eObjId, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecObject[(UINT)eObjId].push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Add_GameObjectPool(OBJID eObjId, CGameObject* pGameObject, _int iCount)
{


	return S_OK;
}

HRESULT CLayer::SetOn_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, _int iCnt)
{
	CGameObject* pGameObject = nullptr;

	//오브젝트 풀이 비어있으면
	if (m_vecObjectPool[(_uint)eObjId].empty())
	{
		//횟수만큼 추가
		for (size_t i = 0; i < iCnt; i++)
		{
			pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(eObjPrototype);

			if (nullptr == pGameObject)
				return E_FAIL;

			pGameObject->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
			//스테이지에서 사라진 오브젝트는 별도 처리로 다시 오브젝트 풀로 돌아온다
			m_vecPendingObject[(UINT)eObjId].push_back(pGameObject);		//여기에 임시 저장해두고
		}
	}
	else
	{
		//중복 코드 추후 함수화
		if (m_vecObjectPool[(_uint)eObjId].capacity() >= iCnt)
		{
			for (size_t i = 0; i < iCnt; i++)
			{
				pGameObject = m_vecObjectPool[(_uint)eObjId].back();
				m_vecPendingObject[(_uint)eObjId].pop_back();

				m_vecObject[(UINT)eObjId].push_back(pGameObject);
			}
		}
		else if (m_vecObjectPool[(_uint)eObjId].capacity() < iCnt)
		{
			_int additionalCnt = iCnt - m_vecObjectPool[(_uint)eObjId].capacity();

			for (size_t i = 0; i < iCnt; i++)
			{
				pGameObject = m_vecObjectPool[(_uint)eObjId].back();
				m_vecObjectPool[(_uint)eObjId].pop_back();

				m_vecPendingObject[(UINT)eObjId].push_back(pGameObject);
			}

			//횟수만큼 추가
			for (size_t i = 0; i < additionalCnt; i++)
			{
				pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(eObjPrototype);

				if (nullptr == pGameObject)
					return E_FAIL;

				pGameObject->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
				//스테이지에서 사라진 오브젝트는 별도 처리로 다시 오브젝트 풀로 돌아온다
				m_vecPendingObject[(UINT)eObjId].push_back(pGameObject);
			}
		}
	}

	return S_OK;
 }

HRESULT CLayer::SetOn_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, _int iCnt, const std::wstring& linkId)
{
	CGameObject* pGameObject = nullptr;
	const unsigned key = (unsigned)eObjId;

	// 오브젝트 풀이 비어있으면
	if (m_vecObjectPool[key].empty())
	{
		// 횟수만큼 추가
		for (size_t i = 0; i < (size_t)iCnt; i++)
		{
			pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(eObjPrototype);
			if (nullptr == pGameObject)
				return E_FAIL;

			pGameObject->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);

			// ★ 기존과 동일: pending에 보관
			m_vecPendingObject[key].push_back(pGameObject);

			// ★ 추가: 태그 “기록”만 (적용은 나중에 Client에서)
			m_mapInteractiveTags[key].push_back({ pGameObject, linkId });
		}
	}
	else
	{
		// 기존 코드 흐름과 동일 (capacity 분기 포함)
		if (m_vecObjectPool[key].capacity() >= (size_t)iCnt)
		{
			for (size_t i = 0; i < (size_t)iCnt; i++)
			{
				pGameObject = m_vecObjectPool[key].back();
				m_vecPendingObject[key].pop_back();

				// 위치 세팅
				pGameObject->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);

				// 실행 컨테이너로 이동
				m_vecObject[key].push_back(pGameObject);

				// ★ 태그 기록
				m_mapInteractiveTags[key].push_back({ pGameObject, linkId });
			}
		}
		else if (m_vecObjectPool[key].capacity() < (size_t)iCnt)
		{
			_int additionalCnt = iCnt - (_int)m_vecObjectPool[key].capacity();

			for (size_t i = 0; i < (size_t)iCnt; i++)
			{
				pGameObject = m_vecObjectPool[key].back();
				m_vecObjectPool[key].pop_back();

				pGameObject->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);

				m_vecPendingObject[key].push_back(pGameObject);

				// ★ 태그 기록
				m_mapInteractiveTags[key].push_back({ pGameObject, linkId });
			}

			// 횟수만큼 추가
			for (size_t i = 0; i < (size_t)additionalCnt; i++)
			{
				pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(eObjPrototype);
				if (nullptr == pGameObject)
					return E_FAIL;

				pGameObject->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
				m_vecPendingObject[key].push_back(pGameObject);

				// ★ 태그 기록
				m_mapInteractiveTags[key].push_back({ pGameObject, linkId });
			}
		}
	}

	return S_OK;
}

void CLayer::PopInteractiveTags(OBJID eObjId, std::vector<SInteractiveTag>& outList)
{
	const unsigned key = (unsigned)eObjId;
	auto it = m_mapInteractiveTags.find(key);
	if (it == m_mapInteractiveTags.end()) return;

	// outList로 이동하고 내부는 비움
	auto& vec = it->second;
	outList.insert(outList.end(),
		std::make_move_iterator(vec.begin()),
		std::make_move_iterator(vec.end()));
	vec.clear();
}

HRESULT CLayer::SetOn_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, CGameObject* _Owner, CGameObject*& _Bullet)
{
	CGameObject* pGameObject = nullptr;

	//오브젝트 풀이 비어있으면
	if (m_mapObjectPool[eObjPrototype].empty())
	{
		//횟수만큼 추가
		pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(eObjPrototype, _Owner);

		_Bullet = pGameObject;

		if (nullptr == pGameObject)
			return E_FAIL;

		pGameObject->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
		//스테이지에서 사라진 오브젝트는 별도 처리로 다시 오브젝트 풀로 돌아온다
		m_vecPendingObject[(UINT)eObjId].push_back(pGameObject);
	}
	else
	{
		//중복 코드 추후 함수화
		if (m_mapObjectPool[eObjPrototype].capacity() >= 1)
		{
			pGameObject = m_mapObjectPool[eObjPrototype].back();
			m_mapObjectPool[eObjPrototype].pop_back();

			_Bullet = pGameObject;

			m_vecPendingObject[(UINT)eObjId].push_back(pGameObject);
		}
		else if (m_mapObjectPool[eObjPrototype].capacity() < 1)
		{
			_int additionalCnt = 1 - m_mapObjectPool[eObjPrototype].capacity();

			pGameObject = m_mapObjectPool[eObjPrototype].back();
			m_mapObjectPool[eObjPrototype].pop_back();

			m_vecPendingObject[(UINT)eObjId].push_back(pGameObject);

			//횟수만큼 추가
			for (size_t i = 0; i < additionalCnt; i++)
			{
				pGameObject = CPrototypeManager::GetInstance()->Clone_ObjPrototype(eObjPrototype, _Owner);
				
				_Bullet = pGameObject;

				if (nullptr == pGameObject)
					return E_FAIL;

				pGameObject->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
				//스테이지에서 사라진 오브젝트는 별도 처리로 다시 오브젝트 풀로 돌아온다
				m_vecPendingObject[(UINT)eObjId].push_back(pGameObject);
			}
		}
	}

	return S_OK;
}

HRESULT CLayer::SetOff_GameLogicObject(_int eObjId, CGameObject* pObj)
{
	//초기화코드 필요 여부 검토
	//m_vecObjectPool[eObjId].push_back(pObj);
	OBJPROTOTYPE eProtoId = pObj->Get_Prototype();

	m_mapObjectPool[eProtoId].push_back(pObj);

	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float& fTimeDelta)
{
	_int iResult(0);

	for (size_t i = 0; i < (size_t)OBJID::OBJ_END; i++)
	{
		for (auto iter = m_vecObject[i].begin(); iter != m_vecObject[i].end(); )
		{
			iResult = (*iter)->Update_GameObject(fTimeDelta); //여기서 생성하고 m_vecObject여기에 푸쉬백 해줌

			if (iResult & 0x80000000)
			{
				SetOff_GameLogicObject(i, (*iter));
				iter = m_vecObject[i].erase(iter);
				//왜 리턴인지 검토
				return iResult;
			}

			++iter;																			// 저기 푸쉬백 해줘서 iter가 무효화 되서 여기 ++ 할때 터지는 상황이였어요
		}

		//업데이트 다 돌고나면 m_vecObject여기에 m_vecPendingObject를 추가해주는 식으로 고쳤더니 

		if (!m_vecPendingObject[i].empty())//그래서 m_vecPendingObject 이 벡터 하나 더 만들어서
		{
			for (auto iter = m_vecPendingObject[i].begin(); iter != m_vecPendingObject[i].end(); iter++)
			{
				m_vecObject[i].push_back(*iter);
			}
			//이거 다요?
			//아마 렉 엄청 걸릴거에요
			//그건 모르겠는데 처음에 이거 안지우니까 매 프레임 계속 추가해줘서 렉걸리는듯요

			for (auto iter = m_vecPendingObject[i].begin(); iter != m_vecPendingObject[i].end(); )
			{
				iter = m_vecPendingObject[i].erase(iter);
			}
			m_vecPendingObject[i].clear();
		}
	}

	return iResult;

}

void CLayer::LateUpdate_Layer(const _float& fTimeDelta)
{
	for (size_t i = 0; i < (size_t)OBJID::OBJ_END; i++)
	{
		for (auto iter = m_vecObject[i].begin(); iter != m_vecObject[i].end(); )
		{
			(*iter)->LateUpdate_GameObject(fTimeDelta);
			++iter;
		}
	}
}

CLayer* CLayer::Create()
{
	CLayer* pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer()))
	{
		MSG_BOX("Layer Create Failed");
		Safe_Release(pLayer);
		return nullptr;
	}

	return pLayer;
}

void CLayer::DeleteObjectGroup(OBJID _eTarget)
{
	auto& objList = m_vecObject[(UINT)_eTarget];

	_ulong dwRefCnt = 0;

	for (CGameObject* pObj : objList)
	{
		if (pObj != nullptr)
		{
			dwRefCnt = pObj->Release();

			if (0 == dwRefCnt)
				pObj = NULL;
		}
	}
	objList.clear();
}

void CLayer::DeleteAll()
{
	SCENEID curSceneId = CManagement::GetInstance()->Get_CurSceneId();

	for (UINT i = 0; i < (UINT)OBJID::OBJ_END; ++i)
	{
		//if (curSceneId != SCENEID::STAGE3
		//	&& i == (_uint)OBJID::OBJ_PLAYER)
		if (curSceneId != SCENEID::TEST
			&& i == (_uint)OBJID::OBJ_PLAYER)
			continue;

		DeleteObjectGroup((OBJID)i);
	}
}

void CLayer::DeletePool()
{
	for (auto& pair : m_mapObjectPool)
	{
		auto& objList = pair.second;

		_ulong dwRefCnt = 0;

		for (CGameObject* pObj : objList)
		{
			if (pObj != nullptr)
			{
				dwRefCnt = pObj->Release();

				if (0 == dwRefCnt)
					pObj = NULL;
			}
		}
		objList.clear();
	}
	m_mapObjectPool.clear();
}

void CLayer::Free()
{
	DeletePool();
	DeleteAll();
}
