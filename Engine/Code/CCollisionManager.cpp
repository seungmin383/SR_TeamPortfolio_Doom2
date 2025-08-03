#include "CCollisionManager.h"

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

	//TODO: 2025-08-03 xz 평면, xy평면 충돌 보정
	//collider manager에서만 obb사용

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
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, fDepth, 0.3f);
			}
			else
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, -fDepth, 0.3f);
			}
		}
		else
		{
			_Src->Get_Owner()->Get_Transform()->Get_Info(INFO_RIGHT, &dir);
			if (dstOwnerPos.x < srcOwnerPos.x)
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, fWidth, 0.3f);
			}	
			else
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, -fWidth, 0.3f);
			}
		}

		if (fDepth > fHeight)
		{
			_Src->Get_Owner()->Get_Transform()->Get_Info(INFO_UP, &dir);
			if (dstOwnerPos.y < srcOwnerPos.y)
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, fHeight, 0.3f);
			}
			else
			{
				_Src->Get_Owner()->Get_Transform()->Move_Pos(&dir, -fHeight, 0.3f);
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
		*_pX = fRadiusX - fWidth + 0.1f;
		*_pY = fRadiusY - fHeight + 0.1f;
		*_pZ = fRadiusZ - fDepth + 0.1f;
		return true;
	}

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

void CCollisionManager::Free()
{
}
