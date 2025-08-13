#include "pch.h"
#include "CBarrelExplosive.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"

CBarrelExplosive::CBarrelExplosive(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_iHP(0), m_iFrame_Barrel(0)
{
}

CBarrelExplosive::CBarrelExplosive(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

CBarrelExplosive::~CBarrelExplosive()
{
}

HRESULT CBarrelExplosive::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_iHP = 1;
    m_iFrame_Barrel = 0;

    m_pTransformCom->Set_Pos(25.f, 0.f, 25.f);
    m_pTransformCom->Set_Scale(_vec3(0.2f, 0.2f, 0.2f));

    // Collider
    m_pColliderCom->Ready_CCollider(
        this,
        _vec3(0, 0, 0),           // 초기 위치(Transform가 실제로 잡아줌)
        _vec3(1.f, 1.f, 1.f),  // 아이템 박스 크기 예시
        _vec3(0, 0.4f, 0),        // 약간 위로
        COL_OBJECT,               // 너희 COLLAYERID에서 아이템 레이어
        COL_BULLET | COL_ATTACK | COL_EXPLOSION     
    );
    m_pColliderCom->Set_Active(true);


    return S_OK;
}

_int CBarrelExplosive::Update_GameObject(const float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);
    Set_OnTerrain();
    Bill_Board();

    m_iFrame_Barrel++;
    if (m_iFrame_Barrel >= 2)
        m_iFrame_Barrel = 0;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_bExploding)
        return -1; // 엔진 상수에 맞춰 사용

    return 0;
}

void CBarrelExplosive::LateUpdate_GameObject(const float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
    //m_pAnimCom->Play_Animation(m_eAnim_Type, 0, fTimeDelta, m_iFrame_Barrel);

    if (m_pColliderCom)
        m_pColliderCom->Late_Update(); // ★ 중요
}

void CBarrelExplosive::Render_GameObject(const _float& fTimeDelta)
{
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_state == State::Exploding) {
        if (m_pTexExplode) m_pTexExplode->Set_Texture(m_iFrame_Explosion);
    }
    else { // Idle
        if (m_pTexIdle)    m_pTexIdle->Set_Texture(m_iFrame_Barrel);
    }
    m_pBufferCom->Render_Buffer();

    //충돌체
    m_pColliderCom->Render(g_colRend);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBarrelExplosive::OnCollision(CCollider* _pOther)
{
}

void CBarrelExplosive::OnCollisionEnter(CCollider* _pOther)
{
    if (!_pOther || m_bExploding) return;
    auto layer = _pOther->Get_ColLayer(); 
    if (layer & (COL_BULLET | COL_ATTACK)) { TakeDamage(10); return; }
    if (layer & COL_EXPLOSION) { TakeDamage(9999); return; } // 연쇄폭발
}

void CBarrelExplosive::OnCollisionExit(CCollider* _pOther)
{
}

void CBarrelExplosive::TakeDamage(int dmg)
{
    if (m_state != State::Idle) return; // 이미 터지는 중이면 무시
    m_iHP -= dmg;
    if (m_iHP <= 0) BeginExplosion();
}

void CBarrelExplosive::BeginExplosion()
{
    m_state = State::Exploding;
    m_animAcc = 0.f;
    m_iFrame_Explosion = 0;

    // 반경 데미지는 첫 프레임에 1회만 주자
    ApplyExplosionDamage();
}

void CBarrelExplosive::ApplyExplosionDamage()
{
    if (m_damageApplied) return;
    m_damageApplied = true;

    // 1) 내 위치 얻기
    _vec3 myPos;
    m_pTransformCom->Get_Pos(&myPos); // ✅ Get_Pos로 수정

    const float R = 4.0f;     // 폭발 반경(원하는 값)
    const float R2 = R * R;

    // 2) 현재 씬에서 몬스터 그룹 벡터 얻기
    Engine::CScene* pScene = Engine::CManagement::GetInstance()->Get_Scene();
    if (!pScene) return;

    // 프로젝트의 OBJID에서 '몬스터' 그룹 enum을 사용하세요.
    // CColliderManager가 내부에서 쓰는 API는 Get_TotalGameObject(OBJID) 입니다. :contentReference[oaicite:4]{index=4}
    const std::vector<Engine::CGameObject*>& monsters =
        pScene->Get_TotalGameObject(OBJID::OBJ_ENEMY); // ← 실제 enum 이름에 맞게 변경

    for (auto* obj : monsters)
    {
        if (!obj || obj->Get_Dead()) continue;

        // 3) 몬스터 위치 얻기
        // 공용 Transform 포인터가 없다면 컴포넌트 맵에서 "Com_Transform"를 꺼내 쓰는 방식도 가능
        Engine::CTransform* pTr =
            dynamic_cast<Engine::CTransform*>(obj->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform"));
        if (!pTr) continue;

        _vec3 targetPos;
        pTr->Get_Pos(&targetPos);

        // 4) 거리 제곱으로 반경 판정
        _vec3 d = targetPos - myPos;
        if (D3DXVec3LengthSq(&d) <= R2)
        {
            // 5) 데미지 주기 (프로젝트의 몬스터 인터페이스에 맞게)
            // 예: dynamic_cast<IMonster*>(obj)->TakeDamage(60);
        }
    }
}

HRESULT CBarrelExplosive::Add_Component()
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
    // ★ Idle 텍스처
    m_pTexIdle = dynamic_cast<Engine::CTexture*>(
        CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Barrel"));
    if (!m_pTexIdle) return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_TexIdle", m_pTexIdle });

    // ★ 폭발 텍스처
    m_pTexExplode = dynamic_cast<Engine::CTexture*>(
        CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_BarrelExplosion"));
    if (!m_pTexExplode) return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_TexExpl", m_pTexExplode });



    pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
        (CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
        (CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    OutputDebugStringA("[Stage] add OBJ_ITEM: Health_Small\n");

    return S_OK;
}

void CBarrelExplosive::Set_OnTerrain()
{
    if (!m_pTransformCom || !m_pCalculatorCom) return;

    auto* pTerrainBuf = static_cast<CTerrainTex*>(
        CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));
    if (!pTerrainBuf) return;

    _vec3 pos; m_pTransformCom->Get_Pos(&pos);

    // 그리드 경계(0..(VTXCNT-1)*VTXITV) 밖이면 보정 건너뜀 (혹은 clamp)
    const float maxX = (VTXCNTX - 1) * VTXITV - 1e-3f;
    const float maxZ = (VTXCNTZ - 1) * VTXITV - 1e-3f;
    if (pos.x < 0.f || pos.z < 0.f || pos.x > maxX || pos.z > maxZ) return;

    float h = m_pCalculatorCom->Compute_HeightOnTerrain(&pos, pTerrainBuf->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

    // 살짝 띄워서 지면 간섭 방지
    m_pTransformCom->Set_Pos(pos.x, h + 0.7f, pos.z);
}

void CBarrelExplosive::Bill_Board()
{
    if (!m_pTransformCom)
        return;

    _matrix matWorld, matView, matBill;
    m_pTransformCom->Get_World(&matWorld);
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

    // 빌보드 행렬의 스케일과 위치는 월드 행렬의 것을 그대로 사용                          
    matBill = matWorld;

    // 뷰행렬에서 역행렬을 이용해 카메라의 회전값을 구하고, 빌보드 행렬의 회전값으로 설정  
    D3DXMatrixInverse(&matBill, NULL, &matView);

    // 빌보드 행렬의 위치값을 월드 행렬의 위치값으로 설정                                  
    matBill._41 = matWorld._41;
    matBill._42 = matWorld._42;
    matBill._43 = matWorld._43;

    // 최종적으로 빌보드 행렬을 월드 행렬로 설정                                           
    m_pTransformCom->Set_World(&matBill);
}

CBarrelExplosive* CBarrelExplosive::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBarrelExplosive* pBarrel = new CBarrelExplosive(pGraphicDev);

    return pBarrel;
}

CBarrelExplosive* CBarrelExplosive::Clone()
{
    CBarrelExplosive* pBE = new CBarrelExplosive(*this);

    if (FAILED(pBE->Ready_GameObject()))
    {
        Safe_Release(pBE);
        MSG_BOX("Barrel Create Failed");
        return nullptr;
    }

    return pBE;
}

void CBarrelExplosive::Free()
{
    Engine::CGameObject::Free();
}
