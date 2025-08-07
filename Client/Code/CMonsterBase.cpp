#include "pch.h"
#include "CMonsterBase.h"

#include <iostream>


CMonsterBase::CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev), m_fSightAngle(0.f), m_fSightDistance(0.f)
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
	:CCharacter(rhs), m_fSightAngle(m_fSightAngle), m_fSightDistance(m_fSightDistance)
{
}

CMonsterBase::~CMonsterBase()
{
}

void CMonsterBase::Debug_Line(_vec3 _Pos, _vec3 _Point1, _vec3 _Point2)
{
	VTXCOL pVertex[3] = {
	{ _Pos, D3DCOLOR_ARGB(255, 255, 255, 255) },
	{ _Point2, D3DCOLOR_ARGB(255, 255, 255, 255)},
	{ _Point1, D3DCOLOR_ARGB(255, 255, 255, 255)}
	};

	/*VTXCOL pVertex[3] = {
		{ _vec3{ -1.0f, -1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) },
		{ _vec3{  1.0f, -1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) },
		{ _vec3{  0.0f,  1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) }
	};*/

	m_pGraphicDev->SetFVF(FVF_COL); // 정점 포맷 설정
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pVertex, sizeof(VTXCOL));
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

_vec3 CMonsterBase::Set_Direction(_vec3* _Look)
{
	if(_Look == nullptr)
		m_vtmpLook = *_Look;

	//랜덤으로 0~7의 숫자를 뽑기
	_int RandMove = rand() % 8;
	_matrix RotMat;
	D3DXMatrixIdentity(&RotMat);

	//각 숫자에 맞는 방향으로 이동하기
	switch (RandMove)
	{
	//앞으로
	case 0:
		break;

	//왼쪽 앞으로
	case 1:
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(45.f));
		break;

	//왼쪽으로
	case 2: 
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(90.f));
		break;

	//왼쪽 뒤로
	case 3:
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(135.f));
		break;

	//뒤로
	case 4:
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(180.f));
		break;

	//오른쪽 뒤로
	case 5: 
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(-135.f));
		break;

	//오른쪽으로
	case 6:
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(-90.f));
		break;

	//오른쪽 앞으로
	case 7:
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(-45.f));
		break;

	default:
		break;
	}

	D3DXVec3Normalize(&m_vtmpLook, &m_vtmpLook);
	D3DXVec3TransformCoord(&m_vtmpLook, &m_vtmpLook, &RotMat);
	if(_Look != nullptr)
		*_Look = m_vtmpLook;

	return m_vtmpLook;
}

_float CMonsterBase::Calc_AngleToPlayer(_vec3 vLook, _vec3 vPos)
{
	// 몬스터 위치에서 플레이어 위치를 뺴서 플레이어가 몬스터를 바라보는 벡터를 만들기
	_vec3 PlayerPos;
	_matrix CameraMat;
	D3DXMatrixIdentity(&CameraMat);

	//뷰행렬 가져오고 역행렬로 만들어서 카메라의 월드 행렬을 만듬
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &CameraMat);
	D3DXMatrixInverse(&CameraMat, NULL, &CameraMat);

	//카메라의 위치를 가져오고
	PlayerPos.x = CameraMat._41;
	PlayerPos.y = CameraMat._42;
	PlayerPos.z = CameraMat._43;

	//플레이어가 몬스터를 바라보는 방향의 벡터를 구함
	_vec3 PlayerLook = PlayerPos - vPos;

	//정규화 해주고
	D3DXVec3Normalize(&PlayerLook, &PlayerLook);
	D3DXVec3Normalize(&vLook, &vLook);
	//D3DXVec3Normalize(&vPos, &vPos); //미친 위치벡터를 정규화 했네
	
	//_float Angle;
	//Angle = acosf( D3DXVec3Dot(&PlayerLook, &vLook));

	float angleLook = atan2f(vLook.z, vLook.x);
	float angleToPlayer = atan2f(PlayerLook.z, PlayerLook.x);

	float fAngle = D3DXToDegree(angleToPlayer - angleLook);

	//_vec3 vCross;
	//D3DXVec3Cross(&vCross, &vLook, &PlayerLook);

	//Angle = D3DXToDegree(Angle);
	//if (vCross.y < 0.f)
	//	Angle = -Angle;

	//std::cout << "Angle : " << Angle << std::endl;
	//return Angle;
	// 각도 정규화: -180 ~ +180
	if (fAngle > 180.f)
		fAngle -= 360.f;
	if (fAngle < -180.f)
		fAngle += 360.f;

	std::cout << "Angle: " << fAngle << std::endl;
	return fAngle;
}

_int CMonsterBase::Calc_AnimState(_vec3 vLook, _vec3 vPos)
{
	_float fAngle = Calc_AngleToPlayer(vLook, vPos);

	if (fAngle >= 0.f && fAngle < 30.f)
		return MONSTER_ANIM_MOVE_F;
	else if (fAngle >= 30.f && fAngle < 60.f)
		return MONSTER_ANIM_MOVE_FR;
	else if (fAngle >= 60.f && fAngle < 90.f)
		return MONSTER_ANIM_MOVE_R;
	else if (fAngle >= 90.f && fAngle < 120.f)
		return MONSTER_ANIM_MOVE_R;
	else if (fAngle >= 120.f && fAngle < 150.f)
		return MONSTER_ANIM_MOVE_BR;
	else if (fAngle >= 150.f && fAngle <= 180.f)
		return MONSTER_ANIM_MOVE_B;

	else if (fAngle < 0.f && fAngle >= -30.f)
		return MONSTER_ANIM_MOVE_F;
	else if (fAngle < -30.f && fAngle >= -60.f)
		return MONSTER_ANIM_MOVE_FL;
	else if (fAngle < -60.f && fAngle >= -90.f)
		return MONSTER_ANIM_MOVE_L;
	else if (fAngle < -90.f && fAngle >= -120.f)
		return MONSTER_ANIM_MOVE_L;
	else if (fAngle < -120.f && fAngle >= -150.f)
		return MONSTER_ANIM_MOVE_BL;
	else if (fAngle < -150.f && fAngle >= -180.f)
		return MONSTER_ANIM_MOVE_B;

}

void CMonsterBase::Free()
{
	Engine::CCharacter::Free();
}