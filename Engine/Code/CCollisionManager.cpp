#include "CCollisionManager.h"

CCollisionManager::CCollisionManager()
{
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::Collision_Rect_Resolve(CCollider* _Dst, CCollider* _Src)
{
	if (nullptr == _Dst || nullptr == _Src)
		return;

	if (_Dst == _Src)
		return;

	float fWidth(0.f), fHeight(0.f), fDepth(0.f);
	if (Check_Mask(_Dst, _Src)
		&& Check_Rect(_Dst, _Src, &fWidth, &fHeight, &fDepth))
	{
		auto srcOwner = _Src->Get_Owner();
		//TODO: 2025-07-31
		// GameObject에서 포지션 가져오는 방법 강구
		//_vec3 vSrcOwnerPos = srcOwner->Get_Pos();
	}
}

void CCollisionManager::Collision_Wall_Resolve(CCollider* _Dst, CCollider* _Src)
{
}

bool CCollisionManager::Check_Rect(CCollider* _Dst, CCollider* _Src, _float* _pX, _float* _pY, _float* _pZ)
{
	return false;
}

bool CCollisionManager::Check_AABB(CCollider* _Dst, CCollider* _Src)
{
	return false;
}

bool CCollisionManager::Check_OBB(CCollider* _Dst, CCollider* _Src)
{
	return false;
}

bool CCollisionManager::Check_Mask(CCollider* _Dst, CCollider* _Src)
{
	return ((_Dst->Get_ColLayer() & _Src->Get_ColMask()) != 0 && (_Src->Get_ColLayer() & _Dst->Get_ColMask()) != 0);
}

void CCollisionManager::Free()
{
}
