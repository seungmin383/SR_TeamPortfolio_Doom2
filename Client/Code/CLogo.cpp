#include "pch.h"
#include "CLogo.h"
#include "CPrototypeManager.h"

#include "CBackGround.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CTerrain.h"

#include "CStage.h"

#include "CManagement.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (GetAsyncKeyState(VK_RETURN))
	{
		Engine::CScene* pScene = CStage::Create(m_pGraphicDev);
		if (nullptr == pScene)
			return -1;

		if (FAILED(CManagement::GetInstance()->Set_Scene(pScene)))
			return -1;
	}

	return iExit;
}

void CLogo::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CLogo::Render_Scene()
{
	// Debug¿ë ·»´õ
}

HRESULT CLogo::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;
	
	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CBackGround::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"BackGround", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{
	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_RectTex", Engine::CRectTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CPrototypeManager::GetInstance()->Ready_Prototype(L"Proto_LogoTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Logo/IU.jpg", TEX_NORMAL))))
		return E_FAIL;

	return S_OK;
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo* pLogo = new CLogo(pGraphicDev);

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
	Engine::CScene::Free();
}
