#include "pch.h"
#include "CGameUI_Face.h"
#include "CDynamicCamera.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"

CGameUI_Face::CGameUI_Face(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_lFrame(0.f), m_pCamera(nullptr)
	, m_bIsEvil(false), m_fEvilTime(0.f)
{
}

CGameUI_Face::CGameUI_Face(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CGameUI_Face::~CGameUI_Face()
{
}

HRESULT CGameUI_Face::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_lFrame = 0.f;

	// 원본 텍스쳐의 크기와 비율을 정의
	const _float fTexWidth = 32.f;
	const _float fTexHeight = 31.f;
	const _float fTexAspectRatio = fTexWidth / fTexHeight;

	_vec3 vPos, vScale;

	// UI의 너비를 화면의 너비로 설정
	vScale.x = 68.f;
	vScale.y = vScale.x / fTexAspectRatio;		// 화면 너비를 10으로 나눈 값
	vScale.z = 1.f;

	// UI의 위치를 화면 맨 아래로 설정
	// UI의 중심점은 화면 하단 + UI 높이의 절반 위치에 둔다.
	vPos = { 0.f, -WINCY * 0.5f + vScale.y + 15.f, 0.f };

	// 계산된 위치와 스케일을 Transform 컴포넌트에 적용
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Set_Scale(vScale);

	m_pCamera = dynamic_cast<CDynamicCamera*>(CManagement::GetInstance()->Get_Player());
	if (!m_pCamera) return E_FAIL;

	return S_OK;
}

_int CGameUI_Face::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsEvil)
	{
		m_fEvilTime -= fTimeDelta;
		if (m_fEvilTime <= 0.f)
		{
			m_bIsEvil = false;
			m_fEvilTime = 0.f;
		}
	}

	CGameObject::Update_GameObject(fTimeDelta);


	// 마우스 이동방향에 따라 얼굴 애니메이션 체크
	_long lMouseMoveX = CDynamicCamera::s_lMouseMoveX;

	if (lMouseMoveX < 0)
		m_lFrame = 0.f;			// 좌
	else if (lMouseMoveX > 0)
		m_lFrame = 2.f;			// 우
	else
		m_lFrame = 1.f;			// 정면

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PLAYERUI, this);



	return 0;
}

void CGameUI_Face::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGameUI_Face::Render_GameObject(const _float& fTimeDelta)
{
	// 1. 렌더링에 방해가 되는 컬링 옵션을 끕니다.                                                                            
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 2. 기존의 3D 카메라용 View, Projection 행렬을 백업합니다.                                                              
	_matrix matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	// 3. UI 렌더링을 위해 View/Projection 행렬을 새로 설정합니다.                                                            
	_matrix matWorld, matIdentity, matOrtho;

	// UI의 위치와 크기를 담은 월드 행렬을 가져옵니다.                                                                        
	matWorld = *m_pTransformCom->Get_World();

	// View 행렬을 단위 행렬로 만들어 카메라의 위치, 회전 효과를 제거합니다.                                                  
	D3DXMatrixIdentity(&matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matIdentity);

	// 화면 중앙을 (0,0)으로 하는 직교 투영 행렬을 생성하여 원근감을 없애고 2D처럼 만듭니다.                                  
	// WINCX, WINCY는 실제 윈도우의 너비와 높이여야 합니다.                                                                   
	D3DXMatrixOrthoOffCenterLH(&matOrtho, -WINCX * 0.5f, WINCX * 0.5f, -WINCY * 0.5f, WINCY * 0.5f, 0.f, 1.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOrtho);

	// 4. UI를 렌더링합니다.                                                                                                  
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	// 알파 블렌딩을 활성화하여 투명한 부분이 처리되도록 합니다.                                                              
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// 4-1) 기본 표정 먼저 그리기 (HP & 방향 프레임)
	const int hp = m_pCamera->Get_Hp();
	if (hp >= 80)           m_pTextureCom->Set_Texture((int)m_lFrame);
	else if (hp >= 60)      m_pTextureCom1->Set_Texture((int)m_lFrame);
	else if (hp >= 40)      m_pTextureCom2->Set_Texture((int)m_lFrame);
	else if (hp >= 20)      m_pTextureCom3->Set_Texture((int)m_lFrame);
	else                    m_pTextureCom4->Set_Texture((int)m_lFrame);
	m_pBufferCom->Render_Buffer();

	// 4-2) (옵션) 사망 오버레이
	if (hp <= 0 && m_pTextureCom6) {
		m_pTextureCom6->Set_Texture(0);
		m_pBufferCom->Render_Buffer();
	}

	// 4-3) EVIL 오버레이 (픽업 효과) — 가장 도드라지는 컷으로 고정
	if (m_bIsEvil && m_pTextureCom5) {
		const int hp = m_pCamera->Get_Hp();
		if (hp >= 80)           m_pTextureCom5->Set_Texture(0);
		else if (hp >= 60)      m_pTextureCom5->Set_Texture(1);
		else if (hp >= 40)      m_pTextureCom5->Set_Texture(2);
		else if (hp >= 20)      m_pTextureCom5->Set_Texture(3);
		else                    m_pTextureCom5->Set_Texture(4);
		m_pBufferCom->Render_Buffer();
		//OutputDebugStringA("[FACE] EVIL overlay draw\n"); // 필요시 켜서 확인
	}

	// 5. 렌더링이 끝난 후, 백업해두었던 원래의 View, Projection 행렬로 복원합니다.                                           
	// 이렇게 해야 다른 3D 오브젝트들이 정상적으로 렌더링됩니다.                                                              
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	// 6. 렌더링 상태를 원래대로 복원합니다.                                                                                  
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CGameUI_Face::OnCollision(CCollider* _pOther)
{
}

void CGameUI_Face::OnCollisionEnter(CCollider* _pOther)
{
	Set_EvilFace(3.f);
}

void CGameUI_Face::OnCollisionExit(CCollider* _pOther)
{
}

void CGameUI_Face::Set_EvilFace(const _float& fTime)
{
	m_bIsEvil = true;
	m_fEvilTime = fTime;
	OutputDebugStringA("[FACE] Set_EvilFace called\n");   // ← 확인용(원하면 빼도 됨)
}

HRESULT CGameUI_Face::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectTex"));
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_FaceUI"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTextureCom1 = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_FaceUI1"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture1", pComponent });

	pComponent = m_pTextureCom2 = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_FaceUI2"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture2", pComponent });

	pComponent = m_pTextureCom3 = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_FaceUI3"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture3", pComponent });

	pComponent = m_pTextureCom4 = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_FaceUI4"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture4", pComponent });


	pComponent = m_pTextureCom5 = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_FaceUI5"));
	if (nullptr == pComponent)
		return E_FAIL;		

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture5", pComponent });


	pComponent = m_pTextureCom6 = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_FaceUI6"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture6", pComponent });

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	return S_OK;
}

CGameUI_Face* CGameUI_Face::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGameUI_Face* pFace = new CGameUI_Face(pGraphicDev);

	if (FAILED(pFace->Ready_GameObject()))
	{
		Safe_Release(pFace);
		MSG_BOX("Player UI Face Create Failed");
		return nullptr;
	}

	return pFace;
}

void CGameUI_Face::Free()
{
	Engine::CGameObject::Free();
}
