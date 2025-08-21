#include "CCalculator.h"
#include "CTerrainTex.h"
#include "CTerrainTexSector.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CCollider.h"

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CCalculator::~CCalculator()
{
}

HRESULT CCalculator::Ready_Calculator()
{
	return S_OK;
}

_float CCalculator::Compute_HeightOnTerrain(const _vec3* pPos,
	const _vec3* pTerrainVtxPos,
	const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv)
{
	if (pPos->z < 0 || pPos->x < 0)
		return 0.f;

	_ulong dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float fWidth = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x / dwVtxItv);
	_float fHeight = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;

	D3DXPLANE plane;

	//우 상단
	if (fWidth > fHeight)
	{
		D3DXPlaneFromPoints(&plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + dwCntX + 1],
			&pTerrainVtxPos[dwIndex + 1]);
	}
	//좌 하단
	else
	{
		D3DXPlaneFromPoints(&plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + 1],
			&pTerrainVtxPos[dwIndex]);
	}

	// ax + by + cz + d = 0
	// by = -ax - cz - d
	// y = (-ax - cz - d) / b
	return (-plane.a * pPos->x - plane.c * pPos->z - plane.d) / plane.b;
}

_vec3 CCalculator::Picking_OnTerrain(HWND hWnd, CTerrainTex* pTerrainBufferCom, CTransform* pTerrainTransformCom)
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// 뷰포트 -> 투영
	_vec3 vMousePos;

	D3DVIEWPORT9 viewPort;
	ZeroMemory(&viewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&viewPort);

	vMousePos.x = ptMouse.x / (viewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(viewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	//투영 -> 뷰스페이스
	D3DXMATRIX matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//뷰스페이스 -> 월드
	D3DXMATRIX matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3 vRayPos{ 0.f, 0.f, 0.f };
	_vec3 vRayDir = vMousePos - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	//월드 -> 로컬
	_matrix matWorld;
	pTerrainTransformCom->Get_World(&matWorld);
	D3DXMatrixInverse(&matWorld, 0, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	_float fU(0.f), fV(0.f), fDist(0.f);
	_ulong dwVtxIdx[3]{};

	const _vec3* pTerrainVtxPos = pTerrainBufferCom->Get_VtxPos();

	for (_ulong i = 0; i < VTXCNTZ; i++)
	{
		for (_ulong j = 0; j < VTXCNTX; j++)
		{
			_ulong dwIndex = i * VTXCNTX + j;

			//오른쪽 위
			dwVtxIdx[0] = dwIndex + VTXCNTX;
			dwVtxIdx[1] = dwIndex + VTXCNTX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			//V1 + U(V2 - V1) + V(V3 - V1)
			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[1]],
				&pTerrainVtxPos[dwVtxIdx[0]],
				&pTerrainVtxPos[dwVtxIdx[2]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtxPos[dwVtxIdx[1]].x + fU * (pTerrainVtxPos[dwVtxIdx[0]].x - pTerrainVtxPos[dwVtxIdx[1]].x),
					0.f,
					pTerrainVtxPos[dwVtxIdx[1]].z + fV * (pTerrainVtxPos[dwVtxIdx[2]].z - pTerrainVtxPos[dwVtxIdx[0]].z));
			}

			//왼쪽 아래
			dwVtxIdx[0] = dwIndex + VTXCNTZ;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[2]],
				&pTerrainVtxPos[dwVtxIdx[1]],
				&pTerrainVtxPos[dwVtxIdx[0]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtxPos[dwVtxIdx[2]].x + fU * (pTerrainVtxPos[dwVtxIdx[1]].x - pTerrainVtxPos[dwVtxIdx[2]].x),
					0.f,
					pTerrainVtxPos[dwVtxIdx[2]].z + fV * (pTerrainVtxPos[dwVtxIdx[0]].z - pTerrainVtxPos[dwVtxIdx[2]].z));
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

_bool CCalculator::Picking_GameObject(HWND hWnd)
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec2 rayScale = { 0.5f, 0.5f };

	// 뷰포트 -> 투영
	_vec3 vMousePos;

	D3DVIEWPORT9 viewPort;
	ZeroMemory(&viewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&viewPort);

	vMousePos.x = ptMouse.x / (viewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(viewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	//투영 -> 뷰스페이스
	D3DXMATRIX matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//뷰스페이스 -> 월드
	D3DXMATRIX matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3 vRayPos{ 0.f, 0.f, 0.f };
	_vec3 vRayDir = vMousePos - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	//월드 -> 로컬
	_matrix matWorld;
	_float fU(0.f), fV(0.f), fDist(0.f);
	_ulong dwVtxIdx[3]{};
	const _vec3* objectColliderBox;

	auto layers = CManagement::GetInstance()->Get_Scene()->Get_Layer();

	for (auto& pair : layers)
	{
		auto objectArrayList = pair.second->Get_AllObject();

		for (size_t i = 0; i < (_uint)OBJID::OBJ_END; i++)
		{
			if (i != (_uint)OBJID::OBJ_ENEMY
				&& i != (_uint)OBJID::OBJ_OBJECT
				&& i != (_uint)OBJID::OBJ_WALL
				&& i != (_uint)OBJID::OBJ_BUTTON)
				continue;

			for (auto object : objectArrayList[i])
			{
				objectColliderBox = object->GetCollider()->Get_WorldCollisionBox();

				//object->Get_Transform()->Get_World(&matWorld);
				//D3DXMatrixInverse(&matWorld, 0, &matWorld);

				//D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
				//D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

				//XY 평면
				//오른쪽 위
				dwVtxIdx[0] = 0;
				dwVtxIdx[1] = 1;
				dwVtxIdx[2] = 3;

				//V1 + U(V2 - V1) + V(V3 - V1)
				if (D3DXIntersectTri(&objectColliderBox[dwVtxIdx[1]],
					&objectColliderBox[dwVtxIdx[0]],
					&objectColliderBox[dwVtxIdx[2]],
					&vRayPos, &vRayDir, &fU, &fV, &fDist))
				{
					object->GetCollider()->HitRay(CManagement::GetInstance()->Get_Player()->GetCollider());
					return true;
				}

				//왼쪽 아래
				dwVtxIdx[0] = 1;
				dwVtxIdx[1] = 2;
				dwVtxIdx[2] = 3;

				if (D3DXIntersectTri(&objectColliderBox[dwVtxIdx[2]],
					&objectColliderBox[dwVtxIdx[1]],
					&objectColliderBox[dwVtxIdx[0]],
					&vRayPos, &vRayDir, &fU, &fV, &fDist))
				{
					object->GetCollider()->HitRay(CManagement::GetInstance()->Get_Player()->GetCollider());
					return true;
				}

				//YZ 평면
				//오른쪽 위
				dwVtxIdx[0] = 0;
				dwVtxIdx[1] = 4;
				dwVtxIdx[2] = 3;

				//V1 + U(V2 - V1) + V(V3 - V1)
				if (D3DXIntersectTri(&objectColliderBox[dwVtxIdx[1]],
					&objectColliderBox[dwVtxIdx[0]],
					&objectColliderBox[dwVtxIdx[2]],

					&vRayPos, &vRayDir, &fU, &fV, &fDist))
				{
					object->GetCollider()->HitRay(CManagement::GetInstance()->Get_Player()->GetCollider());
					return true;
				}

				//왼쪽 아래
				dwVtxIdx[0] = 4;
				dwVtxIdx[1] = 7;
				dwVtxIdx[2] = 3;

				if (D3DXIntersectTri(&objectColliderBox[dwVtxIdx[2]],
					&objectColliderBox[dwVtxIdx[1]],
					&objectColliderBox[dwVtxIdx[0]],
					&vRayPos, &vRayDir, &fU, &fV, &fDist))
				{
					object->GetCollider()->HitRay(CManagement::GetInstance()->Get_Player()->GetCollider());
					return true;
				}

				//XZ 평면
				//오른쪽 위
				dwVtxIdx[0] = 0;
				dwVtxIdx[1] = 1;
				dwVtxIdx[2] = 4;

				//V1 + U(V2 - V1) + V(V3 - V1)
				if (D3DXIntersectTri(&objectColliderBox[dwVtxIdx[1]],
					&objectColliderBox[dwVtxIdx[0]],
					&objectColliderBox[dwVtxIdx[2]],
					&vRayPos, &vRayDir, &fU, &fV, &fDist))
				{
					object->GetCollider()->HitRay(CManagement::GetInstance()->Get_Player()->GetCollider());
					return true;
				}

				//왼쪽 아래
				dwVtxIdx[0] = 1;
				dwVtxIdx[1] = 5;
				dwVtxIdx[2] = 4;

				if (D3DXIntersectTri(&objectColliderBox[dwVtxIdx[2]],
					&objectColliderBox[dwVtxIdx[1]],
					&objectColliderBox[dwVtxIdx[0]],
					&vRayPos, &vRayDir, &fU, &fV, &fDist))
				{
					object->GetCollider()->HitRay(CManagement::GetInstance()->Get_Player()->GetCollider());
					return true;
				}
			}
		}
	}

	return false;
}

_bool CCalculator::Picking_MapTool(HWND hWnd, CTerrainTexSector* pTerrainSectorBufferCom, CTransform* pTerrainTransformCom, _vec3& targetPos)
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec2 rayScale = { 0.5f, 0.5f };

	// 뷰포트 -> 투영
	_vec3 vMousePos;

	D3DVIEWPORT9 viewPort;
	ZeroMemory(&viewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&viewPort);

	vMousePos.x = ptMouse.x / (viewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(viewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	//투영 -> 뷰스페이스
	D3DXMATRIX matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//뷰스페이스 -> 월드
	D3DXMATRIX matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3 vRayPos{ 0.f, 0.f, 0.f };
	_vec3 vRayDir = vMousePos - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	_float fU(0.f), fV(0.f), fDist(0.f);
	_ulong dwVtxIdx[3]{};

	const _vec3* pTerrainVtxPos = pTerrainSectorBufferCom->Get_VtxPos();
	_ulong vtxCntX = pTerrainSectorBufferCom->Get_VtxCntX();
	_ulong vtxCntZ = pTerrainSectorBufferCom->Get_VtxCntZ();
	_ulong vtxItv = pTerrainSectorBufferCom->Get_VtxItv();

	for (_ulong i = 0; i < vtxCntZ; i++)
	{
		for (_ulong j = 0; j < vtxCntX; j++)
		{
			_ulong dwIndex = i * vtxCntX + j;

			//오른쪽 위
			dwVtxIdx[0] = dwIndex + vtxCntX;
			dwVtxIdx[1] = dwIndex + vtxCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			//V1 + U(V2 - V1) + V(V3 - V1)
			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[1]],
				&pTerrainVtxPos[dwVtxIdx[0]],
				&pTerrainVtxPos[dwVtxIdx[2]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				targetPos = pTerrainVtxPos[dwVtxIdx[1]] + (fU * (pTerrainVtxPos[dwVtxIdx[0]] - pTerrainVtxPos[dwVtxIdx[1]])) + (fV * (pTerrainVtxPos[dwVtxIdx[2]] - pTerrainVtxPos[dwVtxIdx[1]]));
				return true;
			}

			//왼쪽 아래
			dwVtxIdx[0] = dwIndex + vtxCntZ;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[2]],
				&pTerrainVtxPos[dwVtxIdx[1]],
				&pTerrainVtxPos[dwVtxIdx[0]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				targetPos = pTerrainVtxPos[dwVtxIdx[1]] + (fU * (pTerrainVtxPos[dwVtxIdx[0]] - pTerrainVtxPos[dwVtxIdx[1]])) + (fV * (pTerrainVtxPos[dwVtxIdx[2]] - pTerrainVtxPos[dwVtxIdx[1]]));
				return true;
			}
		}
	}

	return false;
}

CCalculator* CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator* pCalculator = new CCalculator(pGraphicDev);

	if (FAILED(pCalculator->Ready_Calculator()))
	{
		Safe_Release(pCalculator);
		MSG_BOX("Calculator Create Failed");
		return nullptr;
	}

	return pCalculator;
}

CComponent* CCalculator::Clone()
{
	return new CCalculator(*this);
}

void CCalculator::Free()
{
	CComponent::Free();
}
