#include "pch.h"
#include "CUI_StageClear.h"
#include "CDInputManager.h"
#include "CFontManager.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CStage.h"
#include "CStage2.h"
#include "CStage3.h"

CUI_StageClear* CUI_StageClear::Create(LPDIRECT3DDEVICE9 dev)
{
	CUI_StageClear* pInfo = new CUI_StageClear(dev);

	if (FAILED(pInfo->Ready_GameObject()))
	{
		Safe_Release(pInfo);
		MSG_BOX("UI_StageClear Create Failed");
		return nullptr;
	}
	return pInfo;
}

CUI_StageClear::CUI_StageClear(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CUI_StageClear::CUI_StageClear(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CUI_StageClear::~CUI_StageClear()
{
}

HRESULT CUI_StageClear::Ready_GameObject()
{
	//auto* FM = Engine::CFontManager::GetInstance();
	//FM->Ready_Font(m_pGraphicDev, L"ResultTitle", L"Arial", 28, 32, 900);
	//FM->Ready_Font(m_pGraphicDev, L"ResultBody", L"Arial", 18, 24, 700);

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CUI_StageClear::Update_GameObject(const _float& fTimeDelta)
{
	const auto& cur = CStatsManager::I().Cur();
	m_t += fTimeDelta;

	switch (m_state) {
	case State::Intro:
		if (m_t > 0.4f) { m_state = State::Counting; m_t = 0.f; }
		break;

	case State::Counting: {
		const float nps = 60.f;   // 초당 증가량
		m_k = min(cur.kills, m_k + (int)(nps * fTimeDelta));
		m_i = min(cur.items, m_i + (int)(nps * fTimeDelta));
		//m_s = min(cur.secrets, m_s + (int)(nps * fTimeDelta));
		m_time = min(cur.timeSec, m_time + 120.f * fTimeDelta);

		const bool done = (m_k == cur.kills) && (m_i == cur.items) && (m_s == cur.secrets)
			&& (fabsf(m_time - cur.timeSec) < 0.01f);
		if (done) { m_state = State::Wait; m_t = 0.f; }
		break;
	}

	case State::Wait:
		if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_RETURN) & 0x80) {
			auto* mg = CManagement::GetInstance();
			auto* cur = mg->Get_Scene();   // 현재 씬 포인터
			SCENEID next = SCENEID::STAGE2;

			if (dynamic_cast<CStage2*>(cur))      next = SCENEID::STAGE3;
			else if (dynamic_cast<CStage3*>(cur)) next = SCENEID::MAINMENU; // 끝나면 메인메뉴로

			if (auto* st = dynamic_cast<CStage*>(cur)) {
				st->RequestNextScene(next);  // 즉시 전환 금지! 예약만
			}
			else if (auto* st2 = dynamic_cast<CStage2*>(cur)) {
				st2->RequestNextScene(next);
			}
			else if (auto* st3 = dynamic_cast<CStage3*>(cur)) {
				CManagement::GetInstance()->Change_Scene(next);		// 3스테이지에서 끝나면 메인메뉴로
			}
		}
		break;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CUI_StageClear::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CUI_StageClear::Render_GameObject(const _float& dt)
{
	//// 1. 렌더링에 방해가 되는 컬링 옵션을 끕니다.                                                                            
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//// 2. 기존의 3D 카메라용 View, Projection 행렬을 백업합니다.                                                              
	_matrix matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	//// 3. UI 렌더링을 위해 View/Projection 행렬을 새로 설정합니다.                                                            
	_matrix matWorld, matIdentity, matOrtho;

	//// UI의 위치와 크기를 담은 월드 행렬을 가져옵니다.                                                                        
	matWorld = *m_pTransformCom->Get_World();

	//// View 행렬을 단위 행렬로 만들어 카메라의 위치, 회전 효과를 제거합니다.                                                  
	D3DXMatrixIdentity(&matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matIdentity);

	//// 화면 중앙을 (0,0)으로 하는 직교 투영 행렬을 생성하여 원근감을 없애고 2D처럼 만듭니다.                                  
	//// WINCX, WINCY는 실제 윈도우의 너비와 높이여야 합니다.                                                                   
	D3DXMatrixOrthoOffCenterLH(&matOrtho, -WINCX * 0.5f, WINCX * 0.5f, -WINCY * 0.5f, WINCY * 0.5f, 0.f, 1.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOrtho);

	//// 4. UI를 렌더링합니다.                                                                                                  
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	//// 알파 블렌딩을 활성화하여 투명한 부분이 처리되도록 합니다.                                                              
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	_vec3 vScale;

	vScale.x = 11.f;
	vScale.y = vScale.x / (11.f / 12.f);
	vScale.z = 1.f;

	const auto& cur = CStatsManager::I().Cur();
	auto pct = [](int a, int b) { return b > 0 ? int((a * 100.0) / b + 0.5) : 100; };

	Render_Sprite(m_pTextureClearCom, -60.f, 30.f, 60.f, 12.f);					// 결과 알림 UI

	Render_Sprite(m_pTextureKillsCom, -60.f, 90.f, 60.f, 12.f);
	Render_Number(m_k, 40.f, 90.f, vScale.x, vScale.y);							// 죽인 몬스터 수

	Render_Sprite(m_pTextureItemsCom, -60.f, 120.f, 60.f, 12.f);
	Render_Number(m_i, 40.f, 120.f, vScale.x, vScale.y);						// 먹은 아이템 수

	Render_Sprite(m_pTextureTimesCom, -60.f, 180.f, 60.f, 12.f);
	Render_Number((_int)Get_Time(), 40.f, 180.f, vScale.x, vScale.y);			// 진행 시간

	if (m_state == State::Wait) {
		// "Press Enter" 렌더링
	}

	// 5. 렌더링이 끝난 후, 백업해두었던 원래의 View, Projection 행렬로 복원합니다.                                           
	// 이렇게 해야 다른 3D 오브젝트들이 정상적으로 렌더링됩니다.                                                              
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	// 6. 렌더링 상태를 원래대로 복원합니다.                                                                                  
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// auto* FM = Engine::CFontManager::GetInstance();
	// const auto& cur = CStatsManager::I().Cur();
	// auto pct = [](int a, int b) { return b > 0 ? int((a * 100.0) / b + 0.5) : 100; };

	// /// 간단 헬퍼: 좌표(x,y)를 받으면 내부에서 _vec2를 만들어 포인터로 넘김
	// auto draw = [&](const wchar_t* font, const wchar_t* text, float x, float y, D3DXCOLOR color) {
	//     _vec2 p{ x, y };      // ★ 로컬 l-value
	//     FM->Render_Font(font, text, &p, color);
	//     };

	// draw(L"ResultTitle", L"STAGE CLEAR", 30.f, 30.f, D3DXCOLOR(1, 1, 0, 1));

	// wchar_t line[128];

	// swprintf(line, 128, L"KILLS    %d/%d  %d%%", m_k, cur.totalKills, pct(m_k, cur.totalKills));
	// draw(L"ResultBody", line, 40.f, 90.f, D3DXCOLOR(1, 1, 1, 1));

	// swprintf(line, 128, L"ITEMS    %d/%d  %d%%", m_i, cur.totalItems, pct(m_i, cur.totalItems));
	// draw(L"ResultBody", line, 40.f, 120.f, D3DXCOLOR(1, 1, 1, 1));

	///* swprintf(line, 128, L"SECRETS  %d/%d  %d%%", m_s, cur.totalSecrets, pct(m_s, cur.totalSecrets));
	// draw(L"ResultBody", line, 40.f, 150.f, D3DXCOLOR(1, 1, 1, 1));*/

	// swprintf(line, 128, L"TIME     %.1fs", m_time);
	// draw(L"ResultBody", line, 40.f, 180.f, D3DXCOLOR(0.9f, 0.9f, 0.9f, 1));

	// if (m_state == State::Wait) {
	//     draw(L"ResultBody", L"PRESS ENTER TO CONTINUE", 40.f, 230.f, D3DXCOLOR(1, 1, 1, 1));
	// }
}

HRESULT CUI_StageClear::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectSpriteTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectSpriteTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Number"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Number", pComponent });

	pComponent = m_pTextureClearCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Clear"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Clear", pComponent });

	pComponent = m_pTextureKillsCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Kills"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Kills", pComponent });

	pComponent = m_pTextureItemsCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Items"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Items", pComponent });

	pComponent = m_pTextureTimesCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Times"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Times", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });


	return S_OK;
}

void CUI_StageClear::Render_Number(_int number, _float baseX, _float baseY, _float scaleX, _float scaleY)
{
	wstring numStr = to_wstring(number);
	_float digitWidth = 32.f;           // 가로 크기
	_float spacing = 2.f;              // 자간

	for (size_t i = 0; i < numStr.length(); ++i)
	{
		_int digit = numStr[i] - L'0';
		float x = baseX + i * (digitWidth + spacing);	// 각 자리 위치를 계산 (자릿수 만큼 오른쪽으로 이동)
		float y = baseY;

		// 위치/스케일 갱신 (매번!)
		m_pTransformCom->Set_Pos(x, y, 0.f);
		m_pTransformCom->Set_Scale({ scaleX, scaleY, 1.f });

		// Set_Pos, Set_Scale 등으로 지정한 값을 실제 월드 행렬로 계산하여 적용한다.
		// 이 호출이 없으면 위치/크기 변경이 렌더에 반영되지 않음!
		m_pTransformCom->Update_Component(0.f);

		// 월드 행렬 갱신
		_matrix matWorld = *m_pTransformCom->Get_World();
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		// 텍스처 지정 (0~9)
		m_pTextureCom->Set_Texture(digit);
		m_pBufferCom->Render_Buffer();
	}
}

void CUI_StageClear::Render_Sprite(Engine::CTexture* texture, _float x, _float y, _float scaleX, _float scaleY)
{
	if (!texture || !m_pTransformCom || !m_pBufferCom) return;

	m_pTransformCom->Set_Pos(x, y, 0.f);
	m_pTransformCom->Set_Scale({ scaleX, scaleY, 1.f });
	m_pTransformCom->Update_Component(0.f);

	_matrix matWorld = *m_pTransformCom->Get_World();
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	texture->Set_Texture();
	m_pBufferCom->Render_Buffer();
}

void CUI_StageClear::Free()
{
	Engine::CGameObject::Free();
}
