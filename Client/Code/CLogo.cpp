#include "pch.h"
#include "CLogo.h"
#include "CPrototypeManager.h"
#include "CBackGround.h"
#include "CMonster.h"
#include "CTerrain.h"
#include "CStage.h"
#include "CManagement.h"
//#include "CFontManager.h"

#include "CLoadingBackground.h"
#include "CLoadingSpinner.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev), m_pLoading(nullptr), m_fSleep(0.f)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(LAYERID::LAYER_ENVIRONMENT)))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);

	if (nullptr == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	m_fSleep += fTimeDelta;

	if (m_fSleep > 3.f && true == m_pLoading->Get_Finish())
	{
		CManagement::GetInstance()->Change_Scene(SCENEID::MAINMENU);
	}

	return iExit;
}

void CLogo::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CLogo::Render_Scene(const _float& fTimeDelta)
{
	// Debug용 렌더

	//_vec2 vPos{ 100.f, 100.f };

	//CFontManager::GetInstance()->Render_Font(L"Font_Default", m_pLoading->Get_String(), &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CLogo::Ready_Environment_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	auto background = CLoadingBackground::Create(m_pGraphicDev);
	if (nullptr == background)
		return E_FAIL;

	background->Set_SizeXY(WINCX, WINCY);
	background->Set_Pos(WINCX * 0.5f, WINCY * 0.5f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, background)))
		return E_FAIL;

	auto spinner = CLoadingSpinner::Create(m_pGraphicDev);
	if (nullptr == spinner)
		return E_FAIL;

	spinner->Set_SizeXY(100.f, 100.f);
	spinner->Set_Pos(WINCX * 0.5f, WINCY * 0.5f);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_UI, spinner)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_RectTex", Engine::CRectTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_LogoTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/Title/M_DOOM.png", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_LoadingBackground", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/Skybox/a8604.PNG", TEX_NORMAL))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_BackGroundTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/TITLEPIC.png", TEX_NORMAL))))
		return E_FAIL;

	//스프라이트 버퍼 //Loading에서 이동 소재혁
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_RectSpriteTex", Engine::CRectSpriteTex::Create(m_pGraphicDev))))
		return E_FAIL;

	//애니메이터 컴포넌트 //Loading에서 이동 소재혁
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Animator", Engine::CAnimator::Create(m_pGraphicDev))))
		return E_FAIL;

	//트랜스폼 컴포넌트 //Loading에서 이동 소재혁
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(L"Proto_LoadingSpinnerTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Resource/Texture/DoomTex/Title/LoadingSpinner.png", TEX_NORMAL))))
		return E_FAIL;

	return S_OK;
}

CLogo* CLogo::Standby(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CLogo(pGraphicDev);
}

CLogo* CLogo::Create()
{
	CLogo* pLogo = new CLogo(m_pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		MSG_BOX("Logo Create Failed");
		Safe_Release(pLogo);
		return nullptr;
	}

	return pLogo;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);

	Engine::CScene::Free();
}
