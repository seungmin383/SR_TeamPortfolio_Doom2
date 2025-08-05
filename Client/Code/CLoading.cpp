#include "pch.h"
#include "CLoading.h"
#include "CPrototypeManager.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev),
	m_bFinish(false),
	m_eLoadingID(LOADING_END)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_szLoading, sizeof(m_szLoading));
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL,
		0,
		Thread_Main,
		this,
		0,
		NULL);

	m_eLoadingID = eID;

	return S_OK;
}

_uint CLoading::Loading_ForStage()
{
	lstrcpy(m_szLoading, L"Buffer Component Loading.....................");

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_TriCol", Engine::CTriCol::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_RectCol", Engine::CRectCol::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV, L"../Resource/Texture/Terrain/Height.bmp"))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_RectSpriteTex", Engine::CRectSpriteTex::Create(m_pGraphicDev))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Texture Component Loading.....................");

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/Player%d.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/Terrain/Grass_%d.tga", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_SkyBoxTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/SkyBox/burger%d.dds", TEX_CUBE, 4))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_PlayerWeaponTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/Player/CHAINGUN.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_ZombieMan_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/Monster/ZombieMan/MonsterState%d.png", TEX_NORMAL, 1))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_PlayerUI", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/PLAYERUI.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_FaceUI", Engine::CTexture::Create(m_pGraphicDev, L"../ReSource/Texture/DoomTex/STFST0%d.png", TEX_NORMAL, 3))))
		return E_FAIL;


	lstrcpy(m_szLoading, L"Etc Component Loading.....................");

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_Calculator", Engine::CCalculator::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_Material", Engine::CMaterial::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_Collider", Engine::CCollider::Cretae(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_Animator", Engine::CAnimator::Create(m_pGraphicDev))))
		return E_FAIL;

	m_bFinish = true;

	lstrcpy(m_szLoading, L"Loading Complete");

	return 0;
}

unsigned int __stdcall CLoading::Thread_Main(void* pArg)
{
	CLoading* pLoading = reinterpret_cast<CLoading*>(pArg);

	_uint iFlag(0);

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	// _endthreadex(0);

	return iFlag;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading* pLoading = new CLoading(pGraphicDev);

	if (FAILED(pLoading->Ready_Loading(eID)))
	{
		Safe_Release(pLoading);
		MSG_BOX("Loading Create Failed");
		return nullptr;
	}

	return pLoading;
}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	//WaitForMultipleObjects(3, /*총 3개의 커널 오브젝트를 관찰한다는 의미.*/
	//						hThread, /*쓰레드 핸들을 가지고 있는 배열의 첫 주소*/
	//						TRUE, /*모든 쓰레드가 종료(소멸)가 되기를*/
	//						INFINITE); /*영원히 기다린다*/
	//						// 3번인자가 FALSE인 경우에는 하나라도 종료되기를 기다린다는 의미.
	
	// for (int i = 0; i < 3; ++i)
	// {
	// 	// 해당 쓰레드가 실행한 함수가 종료될 때의 반환 값을 얻어온다.
	// 	GetExitCodeThread(hThread[i], &dwResult);
	// }

	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);
	Safe_Release(m_pGraphicDev);
}
