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

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_TriCol", Engine::CTriCol::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_RectCol", Engine::CRectCol::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV, L"../../Client/Resource/Texture/Terrain/Height.bmp"))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_RectSpriteTex", Engine::CRectSpriteTex::Create(m_pGraphicDev))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Texture Component Loading.....................");

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/Terrain/Grass_%d.tga", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_SkyBoxTexture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/SkyBox/burger%d.dds", TEX_CUBE, 4))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_PlayerWeaponTexture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Player/GUN%d.png", TEX_NORMAL, 5))))
		return E_FAIL;

#pragma region MONSTER_ANIM
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_ZombieMan_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Monster/ZombieMan/MonsterState%d.png", TEX_NORMAL, 26))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_ArchVile_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Monster/Vile/MonsterState%d.png", TEX_NORMAL, 34))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Imp_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Monster/Imp/MonsterState%d.png", TEX_NORMAL, 26))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Pinky_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Monster/Pinky/MonsterState%d.png", TEX_NORMAL, 17))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_SpiderMastermind_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Monster/SpiderMastermind/MonsterState%d.png", TEX_NORMAL, 26))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_ShotgunGuy_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Monster/ShotgunGuy/MonsterState%d.png", TEX_NORMAL, 26))))
		return E_FAIL;

#pragma endregion
#pragma region MONSTER_BULLET

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Zombieman_Bullet_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Effect/ZombiemanBullet/BulletState%d.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Imp_Bullet_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Effect/ImpBullet/BulletState%d.png", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Archvile_Bullet_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Effect/ArchvileBullet/BulletState%d.png", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Boss_Bullet_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Effect/BossBullet/BulletState%d.png", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Pinky_Bullet_Texture", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Effect/PinkyBullet/BulletState%d.png", TEX_NORMAL))))
		return E_FAIL;
#pragma endregion

#pragma region PLAYER_BULLET

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_RocketBullet", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Effect/RocketBullet/ROCKET%d.png", TEX_NORMAL, 9))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_BfgBullet", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Effect/BFGBullet/BFGBULLET%d.png", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_NormalBullet", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Effect/NormalBullet/a6676.png", TEX_NORMAL, 1))))
		return E_FAIL;

#pragma endregion


	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_PlayerUI", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/PLAYERUI.png", TEX_NORMAL))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_PlayerUI_Weapon", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STARMS.png", TEX_NORMAL))))
		return E_FAIL;
	
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_MainMenuUI_Logo", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/M_NEWG.png", TEX_NORMAL))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_MainMenuUI_Exit", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/M_QUITG.png", TEX_NORMAL))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_MainMenuUI_Hover", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/M_SKULL1.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_FaceUI", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STFST0%d.png", TEX_NORMAL, 3))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_FaceUI1", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STFST1%d.png", TEX_NORMAL, 3))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_FaceUI2", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STFST2%d.png", TEX_NORMAL, 3))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_FaceUI3", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STFST3%d.png", TEX_NORMAL, 3))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_FaceUI4", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STFST4%d.png", TEX_NORMAL, 3))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_FaceUI5", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STFEVL%d.png", TEX_NORMAL, 5))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_FaceUI6", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STFDEAD0.png", TEX_NORMAL))))
		return E_FAIL;


	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Key", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/BKEYA%d.png", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_UI_Key", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/STKEYS0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Kills", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/WIOSTK.png", TEX_NORMAL))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Items", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/WIOSTI.png", TEX_NORMAL))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Clear", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/WIF.png", TEX_NORMAL))))
		return E_FAIL;
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Times", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/WITIME.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Elevator_Floor", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/FLAT14.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Number", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STTNUM%d.png", TEX_NORMAL, 10))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Ammo_Number", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STYSNUM%d.png", TEX_NORMAL, 10))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Ammo_Number_Off", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/STGNUM%d.png", TEX_NORMAL, 10))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Heal_Small", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/Heal_Small%d.png", TEX_NORMAL, 4))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Heal_Big", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/MEDIA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Armor_Small", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/Armor_Small%d.png", TEX_NORMAL, 4))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Armor_Big", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/ARM2A%d.png", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Clip", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/CLIPA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Shell", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/SHELA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Rocket", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/ROCKA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Cell", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/CELLA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Suit", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/SUITA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Barrel", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/BAR1A%d.png", TEX_NORMAL, 2))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_BarrelExplosion", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/BEXPA%d.png", TEX_NORMAL, 5))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Door_1", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/DOOR2_4.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Switch_On", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/SW1S1.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Switch_Off", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/SW1S0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Shotgun", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/SHOTA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_ChainGun", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/MGUNA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Rocket_Lanuncher", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/LAUNA0.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_BFG", Engine::CTexture::Create(m_pGraphicDev, L"../../Client/Resource/Texture/DoomTex/Object/BFUGA0.png", TEX_NORMAL))))
		return E_FAIL;
	

	lstrcpy(m_szLoading, L"Etc Component Loading.....................");

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;

	//if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Calculator", Engine::CCalculator::Create(m_pGraphicDev))))
	//	return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Material", Engine::CMaterial::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Collider", Engine::CCollider::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Animator", Engine::CAnimator::Create(m_pGraphicDev))))
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
