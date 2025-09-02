#include "pch.h"	
#include "CDropBulletEffect.h"

CDropBulletEffect::CDropBulletEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CDropBulletEffect::CDropBulletEffect(const CDropBulletEffect& rhs)
	: CEffect(rhs)
{
}

CDropBulletEffect::~CDropBulletEffect()
{
}

HRESULT CDropBulletEffect::Ready_GameObject()
{
	CEffect::Ready_GameObject();

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pAnimCom->Add_Animation(
		0,
		m_pTextureCom,
		m_pBufferCom,
		8,
		1,
		0,
		7,
		0.05f,
		true,
		false
	);

	//m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);65

	m_bDead = FALSE;
	return S_OK;
}

_int CDropBulletEffect::Update_GameObject(const _float& fTimeDelta)
{
	CEffect::Update_GameObject(fTimeDelta);
	Tanpii_Movement(fTimeDelta);

	Bill_Board();

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	if (vPos.y <= -10.f)
		return -1;

	
	return 0;
}

void CDropBulletEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CEffect::LateUpdate_GameObject(fTimeDelta);

	m_pAnimCom->Play_Animation(0, 0, fTimeDelta);
}

void CDropBulletEffect::Render_GameObject(const _float& fTimeDelta)
{
	CEffect::Render_GameObject(fTimeDelta);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();
}

void CDropBulletEffect::Reset_Effect(_vec3 vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Set_Scale(_vec3{ 0.3f, 0.3f, 0.3f });
	m_pAnimCom->SetFrameDefault(); 

	m_bDead = FALSE;

	m_fSpeed = 10.f;		
	m_fUpForce = 0.2f;
	m_fLeftForce = 1.f;

	_matrix matPlayer;
	D3DXMatrixIdentity(&matPlayer);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matPlayer);
	D3DXMatrixInverse(&matPlayer, nullptr, &matPlayer);

	_vec3 vRight; 
	vRight.x = matPlayer._11;
	vRight.y = matPlayer._12;
	vRight.z = matPlayer._13;

	vRight = -vRight;
	D3DXVec3Normalize(&vRight, &vRight);

	_vec3 vUp{ 0.f, 1.f, 0.f };
	D3DXVec3Normalize(&vUp, &vUp);
	_float a = 0.f;
	_float b = 0.f;
	a = rand() % 3;
	b = rand() % 3;

	if (a == 0)
		a = 0.5f;

	m_vDir = (vRight * (m_fLeftForce* a * m_fSpeed)) + (vUp * (m_fUpForce* b * m_fSpeed));
}

void CDropBulletEffect::Tanpii_Movement(const _float& fTimeDelta)
{
	m_vDir.y += m_fGravity * fTimeDelta;

	m_pTransformCom->Move_Pos(&m_vDir, 1.0f, fTimeDelta);

}

CEffect* CDropBulletEffect::Clone()
{
	CEffect* pInstance = new CDropBulletEffect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CDropBulletEffect Clone Failed");
		return nullptr;
	}
	return pInstance;
}

CEffect* CDropBulletEffect::Clone(_vec3 vPos)
{
	CEffect* pInstance = new CDropBulletEffect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CDropBulletEffect Clone Failed");
		return nullptr;
	}

	//pInstance->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);

	return pInstance;
}

HRESULT CDropBulletEffect::Add_Component()
{
	CEffect::Add_Component();

	CComponent* pComponent = nullptr;

	m_pTextureCom = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_DropBulletEffect_Texture"));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

CDropBulletEffect* CDropBulletEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDropBulletEffect* pInstance = new CDropBulletEffect(pGraphicDev);

	if (pInstance == nullptr)
	{
		Safe_Release(pInstance);
		MSG_BOX("CDropBulletEffect Create Failed");
		return nullptr;
	}

	return pInstance;
}
void CDropBulletEffect::Free()
{
	CEffect::Free();
}
