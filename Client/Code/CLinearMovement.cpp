#include "pch.h"
#include "CLinearMovement.h"
#include "CBullet.h"

void CLinearMovement::Ready_Movement(CBullet* _Owner)
{
}

void CLinearMovement::Update_Movement(CBullet* _Owner, const _float fTimeDelta)
{
	_vec3 vDir = _Owner->Get_Dir();
	_vec3 vPos = _Owner->Get_Pos(); 
	_float fSpeed = _Owner->Get_Speed();

	vPos += vDir * fSpeed * fTimeDelta;

	_Owner->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CLinearMovement::Free()
{
}