#include "pch.h"
#include "CMainMenu.h"
#include "CBackGround.h"
#include "CManagement.h"
#include "CFontManager.h"
#include "CTextureManager.h"
#include "CLayer.h"
#include "Define.h"



CMainMenu::CMainMenu(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev), m_pLoading(nullptr)
{
}

CMainMenu::~CMainMenu()
{
}

HRESULT CMainMenu::Ready_Scene()
{
	if (FAILED(Ready_Environment_Layer(LAYERID::LAYER_ENVIRONMENT)))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(LAYERID::LAYER_UI)))
		return E_FAIL;

	UpdateHitRects();
	m_focus = 0; // START

	return S_OK;
}

_int CMainMenu::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

	UpdateHitRects();

	// 마우스 호버 계산
	POINT pt = GetMouseClientPos();
	m_hover = -1;
	if (PtInRect(&m_rcStart, pt)) m_hover = 0;
	else if (PtInRect(&m_rcExit, pt)) m_hover = 1;

	// 키보드 포커스 업데이트
	HandleKeyboard(fTimeDelta);

	// 마우스가 버튼 위에 있으면 포커스를 동기화(자연스러운 UX)
	if (m_hover != -1) m_focus = m_hover;

	// m_hover == 0 이면 메뉴 왼쪽에 해골 이미지 출력

	// 클릭 처리(마우스)
	HandleClick(fTimeDelta);

	return iExit;
}

void CMainMenu::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CMainMenu::Render_Scene(const _float& fTimeDelta)
{
	// ---- 상태 백업 ----
	DWORD oldAlpha, oldLight, oldZ;
	m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlpha);
	m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &oldLight);
	m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &oldZ);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ---- 화면 중앙 기준 배치(히트박스 계산과 동일한 좌표 사용) ----
	if (m_pUISprite)
	{
		m_pUISprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		// START ("NEW GAME")
		if (m_pLogoTex) {
			D3DXMATRIX T; D3DXMatrixTranslation(&T, m_posStart.x, m_posStart.y, 0.f);
			m_pUISprite->SetTransform(&T);
			m_pUISprite->Draw(m_pLogoTex, nullptr, nullptr, nullptr, 0xFFFFFFFF);
		}

		// EXIT ("QUIT GAME")
		if (m_pExitTex) {
			D3DXMATRIX T; D3DXMatrixTranslation(&T, m_posExit.x, m_posExit.y, 0.f);
			m_pUISprite->SetTransform(&T);
			m_pUISprite->Draw(m_pExitTex, nullptr, nullptr, nullptr, 0xFFFFFFFF);
		}

		// 해골 인디케이터(호버가 우선, 없으면 포커스)
		const int idx = (m_hover != -1) ? m_hover : m_focus;
		if (m_pHoverTex && (idx == 0 || idx == 1)) {
			const D3DXVECTOR2 itemSize = (idx == 0) ? m_sizeStart : m_sizeExit;
			const D3DXVECTOR2& itemPos = (idx == 0) ? m_posStart : m_posExit;
			const float skullW = (m_sizeSkull.x > 0.f) ? m_sizeSkull.x : 32.f;
			const float skullH = (m_sizeSkull.y > 0.f) ? m_sizeSkull.y : 32.f;
			const float sx = itemPos.x - skullW - 12.f;
			const float sy = itemPos.y + (itemSize.y - skullH) * 0.5f;

			D3DXMATRIX T; D3DXMatrixTranslation(&T, sx, sy, 0.f);
			m_pUISprite->SetTransform(&T);
			m_pUISprite->Draw(m_pHoverTex, nullptr, nullptr, nullptr, 0xFFFFFFFF);
		}

		m_pUISprite->End();
	}

	// ---- 상태 복구 ----
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlpha);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, oldLight);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, oldZ);
}

HRESULT CMainMenu::Load_PreSet()
{
	return S_OK;
}

HRESULT CMainMenu::Ready_Environment_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CBackGround::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	dynamic_cast<CBackGround*>(pGameObject)->Set_IsMainMenu(true);

	if (FAILED(pLayer->Add_GameObject(OBJID::OBJ_BACKGROUND, pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

POINT CMainMenu::GetMouseClientPos() const
{
	POINT pt; GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	return pt;
}

bool CMainMenu::IsLButtonDown() const
{
	return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
}

void CMainMenu::UpdateHitRects()
{
	RECT rc; GetClientRect(g_hWnd, &rc);
	const float cx = (rc.right - rc.left) * 0.5f;
	const float cy = (rc.bottom - rc.top) * 0.5f;

	// DOOM 스타일: 약간 왼쪽으로 오프셋
	const float maxW = max(m_sizeStart.x, m_sizeExit.x);
	const float anchorX = cx - maxW * 0.25f;   // 화면 중앙 기준 왼쪽으로 살짝
	const float yStart = cy - 110.f;
	const float yExit = cy + 110.f;

	m_posStart = D3DXVECTOR2(anchorX, yStart);
	m_posExit = D3DXVECTOR2(anchorX, yExit);

	m_rcStart = { (LONG)anchorX, (LONG)yStart, (LONG)(anchorX + m_sizeStart.x), (LONG)(yStart + m_sizeStart.y) };
	m_rcExit = { (LONG)anchorX, (LONG)yExit , (LONG)(anchorX + m_sizeExit.x), (LONG)(yExit + m_sizeExit.y) };
}

void CMainMenu::HandleClick(_float fTimeDelta)
{
	// 호버 업데이트
	const POINT pt = GetMouseClientPos();
	m_hover = -1;
	if (PtInRect(&m_rcStart, pt)) m_hover = 0;
	else if (PtInRect(&m_rcExit, pt)) m_hover = 1;

	// 왼클릭 엣지 감지
	const bool lDown = IsLButtonDown();
	if (lDown && !m_prevLDown) {
		if (m_hover == 0) { m_focus = 0; PlaySfx_Select(); ActivateFocused(); }
		else if (m_hover == 1) { m_focus = 1; PlaySfx_Select(); ActivateFocused(); }
	}
	m_prevLDown = lDown;
}

void CMainMenu::HandleKeyboard(_float fTimeDelta)
{
	// 간단한 디바운스
	m_inputLock = max(0.f, m_inputLock - fTimeDelta);
	auto pressOnce = [&](int vk)->bool {
		if (GetAsyncKeyState(vk) & 0x8000) {
			if (m_inputLock <= 0.f) { m_inputLock = 0.15f; return true; }
		}
		return false;
		};

	// ↑ / ↓ 로 포커스 이동
	if (pressOnce(VK_UP)) { m_focus = (m_focus + 1 + 1) % 2; PlaySfx_Move(); }   // 0/1 전환
	if (pressOnce(VK_DOWN)) { m_focus = (m_focus + 1) % 2;     PlaySfx_Move(); }

	// Enter/Space 로 확정
	if (pressOnce(VK_RETURN) || pressOnce(VK_SPACE)) {
		ActivateFocused();
	}
}

void CMainMenu::ActivateFocused()
{
	if (m_focus == 0)
		CManagement::GetInstance()->Change_Scene(SCENEID::STAGE1);
	else
		CManagement::GetInstance()->RequestQuit();
}

void CMainMenu::PlaySfx_Move()
{
#if MENU_ENABLE_SOUND
	// 예시) 엔진에 맞춰 여기를 구현
	// if (auto* sm = CSoundManager::GetInstance())
	//     sm->Play_Sound(L"ui_move.wav", CHANNELID::SOUND_UI, 0.6f);
#endif
}

void CMainMenu::PlaySfx_Select()
{
#if MENU_ENABLE_SOUND
	// 예시) 엔진에 맞춰 여기를 구현
	// if (auto* sm = CSoundManager::GetInstance())
	//     sm->Play_Sound(L"ui_select.wav", CHANNELID::SOUND_UI, 0.8f);
#endif
}

HRESULT CMainMenu::Ready_UI_Layer(LAYERID _eLayerId)
{
	Engine::CLayer* pLayer = CLayer::Create();
	if (pLayer == nullptr)
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pUISprite))) return E_FAIL;

	// 이미 Proto_LogoTexture가 등록돼 있지만, raw 텍스처 포인터 접근이 없다면
	// 한 장만 쓰는 로고라 직접 로드가 가장 간단/안전
	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev,
		L"../Resource/Texture/DoomTex/M_NEWG.png", &m_pLogoTex)))
		return E_FAIL;

	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev,
		L"../Resource/Texture/DoomTex/M_QUITG.png", &m_pExitTex)))
		return E_FAIL;

	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev,
		L"../Resource/Texture/DoomTex/M_SKULL1.png", &m_pHoverTex)))
		return E_FAIL;

	// --- 텍스처 실제 크기 캐시 ---
	if (m_pLogoTex) { D3DSURFACE_DESC d{}; if (SUCCEEDED(m_pLogoTex->GetLevelDesc(0, &d)))  m_sizeStart = D3DXVECTOR2((float)d.Width, (float)d.Height); }
	if (m_pExitTex) { D3DSURFACE_DESC d{}; if (SUCCEEDED(m_pExitTex->GetLevelDesc(0, &d)))  m_sizeExit = D3DXVECTOR2((float)d.Width, (float)d.Height); }
	if (m_pHoverTex) { D3DSURFACE_DESC d{}; if (SUCCEEDED(m_pHoverTex->GetLevelDesc(0, &d))) m_sizeSkull = D3DXVECTOR2((float)d.Width, (float)d.Height); }

	// --- 히트박스/배치 계산 ---
	UpdateHitRects();

	m_mapLayer.insert({ _eLayerId, pLayer });

	return S_OK;
}

HRESULT CMainMenu::Ready_Prototype()
{

	return S_OK;
}

CMainMenu* CMainMenu::Standby(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CMainMenu(pGraphicDev);
}

CMainMenu* CMainMenu::Create()
{
	CMainMenu* pMainMenu = new CMainMenu(m_pGraphicDev);

	if (FAILED(pMainMenu->Ready_Scene()))
	{
		MSG_BOX("MainMenu Create Failed");
		Safe_Release(pMainMenu);
		return nullptr;
	}

	return pMainMenu;
}

void CMainMenu::Free()
{
	Safe_Release(m_pLoading);
	if (m_pHoverTex) { m_pHoverTex->Release(); m_pHoverTex = nullptr; }
	if (m_pExitTex) { m_pExitTex->Release(); m_pExitTex = nullptr; }
	if (m_pLogoTex) { m_pLogoTex->Release();  m_pLogoTex = nullptr; }
	if (m_pUISprite) { m_pUISprite->Release(); m_pUISprite = nullptr; }
	Engine::CScene::Free();
}
