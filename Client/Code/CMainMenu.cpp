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
	m_inputLock = max(0.f, m_inputLock - fTimeDelta);

	_int iExit = CScene::Update_Scene(fTimeDelta);

	UpdateHitRects();

	// 마우스 호버 계산
	POINT pt = GetMouseClientPos();
	m_hover = -1;
	if (PtInRect(&m_rcStart, pt)) m_hover = 0;
	else if (PtInRect(&m_rcExit, pt)) m_hover = 1;

	// 키보드 포커스 업데이트
	HandleKeyboard();

	// 마우스가 버튼 위에 있으면 포커스를 동기화(자연스러운 UX)
	if (m_hover != -1) m_focus = m_hover;
	
	// m_hover == 0 이면 메뉴 왼쪽에 해골 이미지 출력

	// 클릭 처리(마우스)
	HandleClick();

	return iExit;
}

void CMainMenu::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CMainMenu::Render_Scene(const _float& fTimeDelta)
{
	auto* fm = CFontManager::GetInstance();
	if (!fm) return;

	// ===== 0) 화면 중앙 좌표 =====
	RECT rc; GetClientRect(g_hWnd, &rc);
	const float cx = (rc.right - rc.left) * 0.5f;
	const float yCenter = (rc.bottom - rc.top) * 0.5f;

	// ===== 1) 기존 렌더상태 저장 & UI용 상태로 전환 =====
	DWORD oldZ = TRUE, oldLight = TRUE, oldAlpha = FALSE;
	m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &oldZ);
	m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &oldLight);
	m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlpha);

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);          // UI는 깊이비교 끔
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);         // 조명 영향 제거
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);  // 투명도 적용
	
	// 마우스/키보드 포커스 하이라이트
	const bool hotStart = (m_hover == 0) || (m_hover == -1 && m_focus == 0);
	const bool hotExit = (m_hover == 1) || (m_hover == -1 && m_focus == 1);
	D3DXCOLOR colStart = hotStart ? D3DXCOLOR(1.f, 1.f, 0.f, 1.f) : D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	D3DXCOLOR colExit = hotExit ? D3DXCOLOR(1.f, 1.f, 0.f, 1.f) : D3DXCOLOR(1.f, 1.f, 1.f, 1.f);


	// ===== 2) 텍스트 스프라이트 =====
	if (m_pUISprite && m_pLogoTex) {
		const float logoW = 320.f, logoH = 120.f;
		const float logoX = cx - logoW * 0.25f;
		const float logoY = (yCenter - 110.f);     // 중앙 위로 살짝

		m_rcStart.left = 0.f;
		m_rcStart.top = 0.f;
		m_rcStart.right = 200.f;
		m_rcStart.bottom = 40.f;

		D3DXVECTOR3 center(60.0f, 7.5f, 0.0f); // 텍스처 중심
		D3DXVECTOR3 position(logoX, logoY, 0.0f); // 화면상 위치

		D3DXMATRIX T; D3DXMatrixTranslation(&T, logoX, logoY, 0.f);
		m_pUISprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
		m_pUISprite->SetTransform(&T);
		m_pUISprite->Draw(m_pLogoTex, nullptr, nullptr, nullptr, 0xFFFFFFFF);
		m_pUISprite->End();
	}

	if (m_pUISprite && m_pExitTex) {
		const float logoW = 320.f, logoH = 120.f;
		const float logoX = cx - logoW * 0.25f;
		const float logoY = (yCenter + 110.f);     // 중앙 위로 살짝

		m_rcExit.left = 0.f;
		m_rcExit.top = 0.f;
		m_rcExit.right = 260.f;
		m_rcExit.bottom = 40.f;

		D3DXVECTOR3 center(58.0f, 8.5f, 0.0f); // 텍스처 중심
		D3DXVECTOR3 position(logoX, logoY, 0.0f); // 화면상 위치

		D3DXMATRIX T; D3DXMatrixTranslation(&T, logoX, logoY, 0.f);
		m_pUISprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
		m_pUISprite->SetTransform(&T);
		m_pUISprite->Draw(m_pExitTex, nullptr, nullptr, nullptr, 0xFFFFFFFF);
		m_pUISprite->End();
	}

	if (m_hover == 0)
	{

	}
	// ===== 3) 버튼 텍스트(START / EXIT) =====
	const float yStart = yCenter - 30.f;
	const float yExit = yCenter + 30.f;

	_vec2 posStart{ cx - 80.f, yStart - 16.f };
	_vec2 posExit{ cx - 60.f, yExit - 16.f };

	fm->Render_Font(L"ResultTitle", L"START", &posStart, colStart);
	fm->Render_Font(L"ResultTitle", L"EXIT", &posExit, colExit);

	// 조작 힌트(옵션)
	//_vec2 hintPos{ cx - 220.f, (float)rc.bottom - 60.f };
	//fm->Render_Font(L"ResultTitle",
	//	L"↑/↓ 또는 W/S : 이동   Enter : 선택   Esc : 종료",
	//	&hintPos, D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f));

	// ===== 4) 렌더상태 복원 =====
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
	const LONG cx = rc.right - rc.left;
	const LONG cy = rc.bottom - rc.top;

	const LONG w = 260;
	const LONG h = 60;
	const LONG x = cx / 2;
	const LONG yStart = cy / 2 - 30;
	const LONG yExit = cy / 2 + 30;

	m_rcStart = { x - w / 2, yStart - h / 2, x + w / 2, yStart + h / 2 };
	m_rcExit = { x - w / 2, yExit - h / 2, x + w / 2, yExit + h / 2 };
}

void CMainMenu::HandleClick()
{
	if (m_inputLock > 0.f) return; // 입력 차단

	const bool curDown = IsLButtonDown();
	const bool pressed = (curDown && !m_prevLDown);
	m_prevLDown = curDown;
	if (!pressed) return;

	POINT pt = GetMouseClientPos();
	if (PtInRect(&m_rcStart, pt)) { m_focus = 0; PlaySfx_Select(); CManagement::GetInstance()->Change_Scene(SCENEID::STAGE1); }
	else if (PtInRect(&m_rcExit, pt)) { m_focus = 1; PlaySfx_Select(); CManagement::GetInstance()->RequestQuit();
	}
}

void CMainMenu::HandleKeyboard()
{
	if (m_inputLock > 0.f) return; // 입력 차단

	auto now = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };

	const bool upNow = now(VK_UP);
	const bool downNow = now(VK_DOWN);
	const bool wNow = now('W');
	const bool sNow = now('S');
	const bool enterNow = now(VK_RETURN);
	const bool escNow = now('O');

	const bool upEdge = upNow && !m_keyPrev.up;    m_keyPrev.up = upNow;
	const bool downEdge = downNow && !m_keyPrev.down;  m_keyPrev.down = downNow;
	const bool wEdge = wNow && !m_keyPrev.w;     m_keyPrev.w = wNow;
	const bool sEdge = sNow && !m_keyPrev.s;     m_keyPrev.s = sNow;
	const bool enterEdge = enterNow && !m_keyPrev.enter; m_keyPrev.enter = enterNow;
	const bool escEdge = escNow && !m_keyPrev.esc;   m_keyPrev.esc = escNow;

	const int count = 2;
	if (upEdge || wEdge) { m_focus = (m_focus - 1 + count) % count; PlaySfx_Move(); }
	if (downEdge || sEdge) { m_focus = (m_focus + 1) % count;         PlaySfx_Move(); }
	if (enterEdge) { PlaySfx_Select(); ActivateFocused(); }
	if (escEdge) { CManagement::GetInstance()->RequestQuit();
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
