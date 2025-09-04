#include "CCollisionManager.h"
#include "CManagement.h"
#include "CTerrainTexSector.h"

CCollisionManager::CCollisionManager()
{
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::Collision_Rect_Resolve(CCollider* _Dst, CCollider* _Src, const _float& fTimeDelta)
{
	if (nullptr == _Dst || nullptr == _Src)
		return;

	if (_Dst == _Src)
		return;

	float fWidth(0.f), fHeight(0.f), fDepth(0.f);
	if (Check_Mask(_Dst, _Src)
		&& Check_AABB(_Dst, _Src, &fWidth, &fHeight, &fDepth))
	{
		_vec3 dstOwnerPos;
		_vec3 srcOwnerPos;
		_Dst->Get_Owner()->Get_Transform()->Get_Pos(&dstOwnerPos);
		_Src->Get_Owner()->Get_Transform()->Get_Pos(&srcOwnerPos);

		_vec3 dir;
		if (fWidth > fDepth)
		{
			_Src->Get_Owner()->Get_Transform()->Get_Info(INFO_LOOK, &dir);
			if (dstOwnerPos.z < srcOwnerPos.z)
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, fDepth, fTimeDelta);
			}
			else
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, -fDepth, fTimeDelta);
			}
		}
		else
		{
			_Src->Get_Owner()->Get_Transform()->Get_Info(INFO_RIGHT, &dir);
			if (dstOwnerPos.x < srcOwnerPos.x)
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, -fWidth, fTimeDelta);
			}
			else
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, fWidth, fTimeDelta);
			}
		}

		if (fDepth > fHeight)
		{
			_Src->Get_Owner()->Get_Transform()->Get_Info(INFO_UP, &dir);
			if (dstOwnerPos.y < srcOwnerPos.y)
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, -fHeight, fTimeDelta);
			}
			else
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, fHeight, fTimeDelta);
			}
		}
	}
}

void CCollisionManager::Collision_OBB_Resolve(CCollider* _Dst, CCollider* _Src)
{
}

bool CCollisionManager::Check_AABB(CCollider* _Dst, CCollider* _Src, _float* _pX, _float* _pY, _float* _pZ)
{
	_vec3 vDstPos = _Dst->Get_FinalPos();
	_vec3 vDstScale = _Dst->Get_Scale();

	_vec3 vSrcPos = _Src->Get_FinalPos();
	_vec3 vSrcScale = _Src->Get_Scale();

	//AABB
	float fRadiusX = (vDstScale.x + vSrcScale.x) * 0.5f;
	float fRadiusY = (vDstScale.y + vSrcScale.y) * 0.5f;
	float fRadiusZ = (vDstScale.z + vSrcScale.z) * 0.5f;

	float fWidth = abs(vDstPos.x - vSrcPos.x);
	float fHeight = abs(vDstPos.y - vSrcPos.y);
	float fDepth = abs(vDstPos.z - vSrcPos.z);

	if (fRadiusX > fWidth && fRadiusY > fHeight && fRadiusZ > fDepth )
	{
		*_pX = fRadiusX - fWidth;
		*_pY = fRadiusY - fHeight;
		*_pZ = fRadiusZ - fDepth;
		return true;
	}

	return false;
}

bool CCollisionManager::Check_AABB(CCollider* _Dst, CCollider* _Src)
{
	_vec3 vDstPos = _Dst->Get_FinalPos();
	_vec3 vDstScale = _Dst->Get_Scale();

	_vec3 vSrcPos = _Src->Get_FinalPos();
	_vec3 vSrcScale = _Src->Get_Scale();

	//AABB
	float fRadiusX = (vDstScale.x + vSrcScale.x) * 0.5f;
	float fRadiusY = (vDstScale.y + vSrcScale.y) * 0.5f;
	float fRadiusZ = (vDstScale.z + vSrcScale.z) * 0.5f;

	float fWidth = abs(vDstPos.x - vSrcPos.x);
	float fHeight = abs(vDstPos.y - vSrcPos.y);
	float fDepth = abs(vDstPos.z - vSrcPos.z);

	if (fRadiusX > fWidth && fRadiusY > fHeight && fRadiusZ > fDepth)
		return true;

	return false;
}

bool CCollisionManager::Check_OBB(CCollider* _Dst, CCollider* _Src)
{
	const _vec3* dstCollider = _Dst->Get_WorldCollisionBox();
	const _vec3* srcCollider = _Src->Get_WorldCollisionBox();

	_vec3 dstAxis[3] = { dstCollider[0] - dstCollider[1],
						dstCollider[0] - dstCollider[3],
						dstCollider[0] - dstCollider[4] };

	_vec3 srcAxis[3] = { srcCollider[0] - srcCollider[1],
						srcCollider[0] - srcCollider[3],
						srcCollider[0] - srcCollider[4] };

	for (_uint i = 0; i < 3; i++)
	{
		_vec3 axis_normal;
		D3DXVec3Normalize(&axis_normal, &dstAxis[i]);

		map<_float, _uint> mValue;
		_uint minIndex, maxIndex;

		for (_uint v = 0; v < 8; v++)
		{
			mValue.insert(make_pair(fabsf(D3DXVec3Dot(&axis_normal, &srcCollider[v])), v));
		}

		map<_float, _uint>::iterator iter;
		iter = mValue.begin();

		for (iter; iter != mValue.end(); iter++) {}
		--iter;

		minIndex = mValue.begin()->second;
		maxIndex = iter->second;

		_float A, B, Dist;
		_vec3 vec;

		switch (i)
		{
		case 0:
			vec = dstCollider[1] - dstCollider[0];
			break;
		case 1:
			vec = dstCollider[3] - dstCollider[0];
			break;
		case 2:
			vec = dstCollider[4] - dstCollider[0];
			break;
		}

		A = fabsf(D3DXVec3Length(&vec));

		_vec3 max = srcCollider[maxIndex];
		_vec3 min = srcCollider[minIndex];

		_float B1 = (D3DXVec3Dot(&axis_normal, &max));
		_float B2 = (D3DXVec3Dot(&axis_normal, &min));

		B = fabsf(B1 - B2);

		A = A * 0.5f;
		B = B * 0.5f;

		_vec3 dstPos = _Dst->Get_FinalPos();
		_vec3 srcPos = _Src->Get_FinalPos();
		B1 = (D3DXVec3Dot(&axis_normal, &dstPos));
		B2 = (D3DXVec3Dot(&axis_normal, &srcPos));
		Dist = fabsf(B1 - B2);

		if (Dist > A + B)
			return false;
	}

	for (_uint i = 0; i < 3; i++)
	{
		_vec3 axis_normal;
		D3DXVec3Normalize(&axis_normal, &srcAxis[i]);

		map<_float, _uint> mValue;
		_uint minIndex, maxIndex;

		for (_uint v = 0; v < 8; v++)
		{
			mValue.insert(make_pair((D3DXVec3Dot(&axis_normal, &srcCollider[v])), v));
		}

		map<_float, _uint>::iterator iter;
		iter = mValue.begin();

		for (iter; iter != mValue.end(); iter++) {}
		--iter;

		minIndex = mValue.begin()->second;
		maxIndex = iter->second;

		_float A, B, Dist;
		_vec3 vec;

		switch (i)
		{
		case 0:
			vec = srcCollider[1] - srcCollider[0];
			break;
		case 1:
			vec = srcCollider[3] - srcCollider[0];
			break;
		case 2:
			vec = srcCollider[4] - srcCollider[0];
			break;
		}

		A = fabsf(D3DXVec3Length(&vec));

		_float B1 = (D3DXVec3Dot(&axis_normal, &dstCollider[maxIndex]));
		_float B2 = (D3DXVec3Dot(&axis_normal, &dstCollider[minIndex]));

		B = fabsf(B1 - B2);

		A = A * 0.5f;
		B = B * 0.5f;

		_vec3 dstPos = _Dst->Get_FinalPos();
		_vec3 srcPos = _Src->Get_FinalPos();
		B1 = (D3DXVec3Dot(&axis_normal, &dstPos));
		B2 = (D3DXVec3Dot(&axis_normal, &srcPos));
		Dist = fabsf(B1 - B2);

		if (Dist > A + B)
			return false;
	}

	return true;
}

bool CCollisionManager::Check_Mask(CCollider* _Dst, CCollider* _Src)
{
	return ((_Dst->Get_ColLayer() & _Src->Get_ColMask()) != 0 && (_Src->Get_ColLayer() & _Dst->Get_ColMask()) != 0);
}

CGameObject* CCollisionManager::Raycast(_vec3 vRayPos, _vec3 vRayDir, _vec3& vHitPos, _vec3& vHitNormal, _float fMaxDist, LAYERID* pHitLayer, CGameObject* _this)
{
	CGameObject* pClosestObj = nullptr;
	_float fU(0.f), fV(0.f), fDist(0.f), fClosestDist(0.f);
	_ulong dwVtxIdx[3]{};
	const _vec3* objectColliderBox;

	// Initialize closest distance to input distance
	fClosestDist = fMaxDist;

	auto layers = CManagement::GetInstance()->Get_Scene()->Get_Layer();

	for (auto& pair : layers)
	{
		if (pair.first == LAYERID::LAYER_GAMELOGIC)
		{
			auto objectArrayList = pair.second->Get_AllObject();

			for (size_t iD = 0; iD < (_uint)OBJID::OBJ_END; iD++)
			{
				if (iD == (_uint)OBJID::OBJ_ENEMY || iD == (_uint)OBJID::OBJ_OBJECT)
				{
					for (auto object : objectArrayList[iD])
					{
						if (object == _this)
							continue;

						objectColliderBox = object->GetCollider()->Get_WorldCollisionBox();

						// Initialize indices
						const _ulong dwIndices[36] =
						{
							// front
							1, 0, 3,	1, 2, 3,
							// back
							4, 5, 6,	4, 7, 6,
							// top
							5, 4, 0,	5, 1, 0,
							// bottom
							6, 7, 3,	6, 2, 3,
							// right
							5, 1, 2,	5, 6, 2,
							// left
							0, 4, 7,	0, 3, 7
						};

						for (int i = 0; i < 12; i++)
						{
							const _vec3& v0 = objectColliderBox[dwIndices[i * 3 + 0]];
							const _vec3& v1 = objectColliderBox[dwIndices[i * 3 + 1]];
							const _vec3& v2 = objectColliderBox[dwIndices[i * 3 + 2]];

							if (D3DXIntersectTri(&v0, &v1, &v2,
								&vRayPos, &vRayDir, &fU, &fV, &fDist))
							{
								if (fClosestDist >= fDist)
								{
									if (pHitLayer != nullptr)
										*pHitLayer = pair.first;

									fClosestDist = fDist;
									pClosestObj = object;

									vHitPos = vRayPos + (vRayDir * fClosestDist);

									// Calculate local normal vector
									_vec3 vEdge1 = objectColliderBox[dwIndices[i * 3 + 0]] - objectColliderBox[dwIndices[i * 3 + 1]];
									_vec3 vEdge2 = objectColliderBox[dwIndices[i * 3 + 2]] - objectColliderBox[dwIndices[i * 3 + 1]];
									D3DXVec3Cross(&vHitNormal, &vEdge1, &vEdge2);
									D3DXVec3Normalize(&vHitNormal, &vHitNormal);
								}
							}
						}
					}
				}
			}
		}

		if (pair.first == LAYERID::LAYER_ENVIRONMENT)
		{
			auto objectArrayList = pair.second->Get_AllObject();

			for (size_t iD = 0; iD < (_uint)OBJID::OBJ_END; iD++)
			{
				if (iD == (_uint)OBJID::OBJ_CEIL ||
					iD == (_uint)OBJID::OBJ_WALL ||
					iD == (_uint)OBJID::OBJ_TERRAIN)
				{
					vector<CGameObject*> innerTerrain;
					for (auto ter : objectArrayList[iD])
						if (InnerTerrainCheck(ter, vRayPos))
							innerTerrain.push_back(ter);

					for (auto object : innerTerrain)
					{
						auto bufferCom = dynamic_cast<CTerrainTexSector*>(object->Get_Component(COMPONENTID::ID_STATIC, L"Com_Buffer"));
						auto bufferTransCom = dynamic_cast<CTransform*>(object->Get_Component(COMPONENTID::ID_DYNAMIC, L"Com_Transform"));

						//월드 -> 로컬
						_matrix matWorld, matInvWorld;
						bufferTransCom->Get_World(&matWorld);
						D3DXMatrixInverse(&matInvWorld, 0, &matWorld);

						_vec3 vLocalRayPos, vLocalRayDir;
						D3DXVec3TransformCoord(&vLocalRayPos, &vRayPos, &matInvWorld);
						D3DXVec3TransformNormal(&vLocalRayDir, &vRayDir, &matInvWorld);

						const _vec3* pTerrainVtxPos = bufferCom->Get_VtxPos();

						_ulong vtxCntX = bufferCom->Get_VtxCntX();
						_ulong vtxCntZ = bufferCom->Get_VtxCntZ();

						for (_ulong i = 0; i < vtxCntZ - 1; ++i)
						{
							for (_ulong j = 0; j < vtxCntX - 1; ++j)
							{
								const _ulong bottomLeft = i * vtxCntX + j;
								const _ulong bottomRight = bottomLeft + 1;
								const _ulong topLeft = bottomLeft + vtxCntX;
								const _ulong topRight = topLeft + 1;

								if (D3DXIntersectTri(&pTerrainVtxPos[bottomLeft],
													 &pTerrainVtxPos[bottomRight],
													 &pTerrainVtxPos[topLeft],
													 &vLocalRayPos, &vLocalRayDir, &fU, &fV, &fDist))
								{
									if (fDist <= fClosestDist)
									{
										if (pHitLayer != nullptr)
											*pHitLayer = pair.first;

										fClosestDist = fDist;
										pClosestObj = object;

										_vec3 vLocalHitPos = vLocalRayPos + (vLocalRayDir * fDist);
										D3DXVec3TransformCoord(&vHitPos, &vLocalHitPos, &matWorld);
										

										// Calculate local normal vector
										_vec3 vEdge1 = pTerrainVtxPos[bottomRight] - pTerrainVtxPos[bottomLeft];
										_vec3 vEdge2 = pTerrainVtxPos[topLeft] - pTerrainVtxPos[bottomLeft];
										D3DXVec3Cross(&vHitNormal, &vEdge1, &vEdge2);
										D3DXVec3TransformNormal(&vHitNormal, &vHitNormal, &matWorld);
										D3DXVec3Normalize(&vHitNormal, &vHitNormal);

										if (D3DXVec3Dot(&vHitNormal, &vRayDir) > 0.f)
											*vHitNormal = -*vHitNormal;
									}
								}

								if (D3DXIntersectTri(&pTerrainVtxPos[topLeft],
													 &pTerrainVtxPos[topRight],
													 &pTerrainVtxPos[bottomRight],
													 &vLocalRayPos, &vLocalRayDir, &fU, &fV, &fDist))
								{
									if (fDist <= fClosestDist)
									{
										if (pHitLayer != nullptr)
											*pHitLayer = pair.first;

										fClosestDist = fDist;
										pClosestObj = object;

										_vec3 vLocalHitPos = vLocalRayPos + (vLocalRayDir * fDist);
										D3DXVec3TransformCoord(&vHitPos, &vLocalHitPos, &matWorld);
										
										// Calculate local normal vector
										_vec3 vEdge1 = pTerrainVtxPos[topLeft] - pTerrainVtxPos[topRight];
										_vec3 vEdge2 = pTerrainVtxPos[bottomRight] - pTerrainVtxPos[topRight];
										D3DXVec3Cross(&vHitNormal, &vEdge1, &vEdge2);
										D3DXVec3TransformNormal(&vHitNormal, &vHitNormal, &matWorld);
										D3DXVec3Normalize(&vHitNormal, &vHitNormal);

										if (D3DXVec3Dot(&vHitNormal, &vRayDir) > 0.f)
											*vHitNormal = -*vHitNormal;
									};
								}
							}
						}
					}
				}
			}


		}
	}
	return pClosestObj;
}

_bool CCollisionManager::InnerTerrainCheck(CGameObject* terrain, _vec3 pos)
{
	auto bufferCom = dynamic_cast<CTerrainTexSector*>(terrain->Get_Component(COMPONENTID::ID_STATIC, L"Com_Buffer"));
	auto transformCom = dynamic_cast<CTransform*>(terrain->Get_Component(COMPONENTID::ID_DYNAMIC, L"Com_Transform"));

	auto vtxpos = bufferCom->Get_VtxPos();
	auto vtxcntx = bufferCom->Get_VtxCntX();
	auto vtxcntz = bufferCom->Get_VtxCntZ();
	const _matrix* w = transformCom->Get_World();

	_vec3 wvtxlu;
	_vec3 wvtxru;
	_vec3 wvtxrb;
	_vec3 wvtxlb;

	_ulong lu = 0;
	_ulong ru = vtxcntx - 1;
	_ulong rb = 0 + ((vtxcntz - 1) * vtxcntx);
	_ulong lb = (vtxcntx - 1) + ((vtxcntz - 1) * vtxcntx);


	D3DXVec3TransformCoord(&wvtxlu, &vtxpos[lu], w);
	D3DXVec3TransformCoord(&wvtxru, &vtxpos[ru], w);
	D3DXVec3TransformCoord(&wvtxrb, &vtxpos[rb], w);
	D3DXVec3TransformCoord(&wvtxlb, &vtxpos[lb], w);

	LONG minx = min(min(wvtxlu.x, wvtxru.x), min(wvtxrb.x, wvtxlb.x));
	LONG maxx = max(max(wvtxlu.x, wvtxru.x), max(wvtxrb.x, wvtxlb.x));
	LONG minz = min(min(wvtxlu.z, wvtxru.z), min(wvtxrb.z, wvtxlb.z));
	LONG maxz = max(max(wvtxlu.z, wvtxru.z), max(wvtxrb.z, wvtxlb.z));

	RECT tile{ minx - 5.f, minz - 5.f, maxx + 5.f, maxz + 5.f };
	RECT posTile{ pos.x - 5.f, pos.z - 5.f, pos.x + 5.f, pos.z + 5.f };
	RECT result;

	return (IntersectRect(&result, &posTile, &tile));
}

_bool CCollisionManager::Check_AABB(RECT myRect)
{
	RECT posTile = CManagement::GetInstance()->Get_RenderSector();
	RECT result;

	return (IntersectRect(&result, &posTile, &myRect));
}

void CCollisionManager::Free()
{
}
