#include "pch.h"
#include "CGameUI_Info_WeaponNum.h"
#include "CManagement.h"
#include "CRenderer.h"

CGameUI_Info_WeaponNum::CGameUI_Info_WeaponNum(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_pCamera(nullptr), m_iNum(0)
{
}

CGameUI_Info_WeaponNum::CGameUI_Info_WeaponNum(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CGameUI_Info_WeaponNum::~CGameUI_Info_WeaponNum()
{
}

HRESULT CGameUI_Info_WeaponNum::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_iNum = 0;


	// 원본 텍스쳐의 크기와 비율을 정의
	const _float fTexWidth = 4.f;
	const _float fTexHeight = 6.f;
	const _float fTexAspectRatio = fTexWidth / fTexHeight;

	_vec3 vPos, vScale;

	// UI의 너비를 화면의 너비로 설정
	vScale.x = 8.f;
	vScale.y = vScale.x / fTexAspectRatio;
	vScale.z = 1.f;

	// UI의 위치를 화면 맨 아래로 설정
	// UI의 중심점은 화면 하단 + UI 높이의 절반 위치에 둔다.
	vPos = { 250.f, -WINCY * 0.5f + vScale.y + 15.f, 0.f };

	// 계산된 위치와 스케일을 Transform 컴포넌트에 적용
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Set_Scale(vScale);

	return S_OK;
}

_int CGameUI_Info_WeaponNum::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PLAYERUI, this);

	return 0;
}

void CGameUI_Info_WeaponNum::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGameUI_Info_WeaponNum::Render_GameObject(const _float& fTimeDelta)
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

	//// 4. UI를 렌더링합니다.                                                                                                  
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	//// 알파 블렌딩을 활성화하여 투명한 부분이 처리되도록 합니다.                                                              
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//m_pBufferCom->Render_Buffer();
	_vec3 vPos, vScale;

	vScale.x = 10.f;
	vScale.y = vScale.x / (4.f / 6.f);		// 화면 너비를 10으로 나눈 값
	vScale.z = 1.f;

	vPos = { -300.f, -WINCY * 0.5f + vScale.y + 15.f, 0.f };

	Render_Number(1, -230.f, -WINCY * 0.5f + vScale.y + 100.f, 10.f, 8.f / (4.f / 6.f));
	Render_Number(2, -170.f, -WINCY * 0.5f + vScale.y + 100.f, 10.f, 8.f / (4.f / 6.f));
	Render_Number(3, -110.f, -WINCY * 0.5f + vScale.y + 100.f, 10.f, 8.f / (4.f / 6.f));


	Render_Number(4, -230.f, -WINCY * 0.5f + vScale.y + 60.f, 10.f, 8.f / (4.f / 6.f));
	Render_Number(5, -170.f, -WINCY * 0.5f + vScale.y + 60.f, 10.f, 8.f / (4.f / 6.f));
	Render_Number(6, -110.f, -WINCY * 0.5f + vScale.y + 60.f, 10.f, 8.f / (4.f / 6.f));

	// 5. 렌더링이 끝난 후, 백업해두었던 원래의 View, Projection 행렬로 복원합니다.                                           
	// 이렇게 해야 다른 3D 오브젝트들이 정상적으로 렌더링됩니다.                                                              
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	// 6. 렌더링 상태를 원래대로 복원합니다.                                                                                  
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CGameUI_Info_WeaponNum::OnCollision(CCollider* _pOther)
{
}

void CGameUI_Info_WeaponNum::OnCollisionEnter(CCollider* _pOther)
{
}

void CGameUI_Info_WeaponNum::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CGameUI_Info_WeaponNum::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Ammo_Number"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTextureCom2 = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Ammo_Number_Off"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Off", pComponent });

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });


	return S_OK;
}

void CGameUI_Info_WeaponNum::Render_Number(int number, float baseX, float baseY, float scaleX, float scaleY)
{
	float x = baseX;	// 각 자리 위치를 계산 (자릿수 만큼 오른쪽으로 이동)
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
	// 만약 플레이어가 안먹었으면 m_pTextureCom2->Set_Texture(number);
	// 아니면 m_pTextureCom->Set_Texture(number);
	m_pTextureCom->Set_Texture(number);
	m_pBufferCom->Render_Buffer();
}

CGameUI_Info_WeaponNum* CGameUI_Info_WeaponNum::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGameUI_Info_WeaponNum* pWeaponNumber = new CGameUI_Info_WeaponNum(pGraphicDev);

	if (FAILED(pWeaponNumber->Ready_GameObject()))
	{
		Safe_Release(pWeaponNumber);
		MSG_BOX("Player Weapon Number Create Failed");
		return nullptr;
	}

	return pWeaponNumber;
}

void CGameUI_Info_WeaponNum::Free()
{
	for_each(m_vecDigitTextures.begin(), m_vecDigitTextures.end(), [](Engine::CTexture* pTexture)
		{
			Safe_Release(pTexture);
		});
	m_vecDigitTextures.clear();

	Engine::CGameObject::Free();
}
