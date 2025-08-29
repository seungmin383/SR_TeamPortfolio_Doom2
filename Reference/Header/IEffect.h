#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL	 IEffect : public CBase
{
protected:
	virtual ~IEffect() PURE;

public:
	virtual void Ready_Effect(const _vec3& vPos) PURE;
	virtual _int Update_Effect(const _float& fTimeDelta) PURE;
	virtual void Render_Effect(const _float& fTimeDelta) PURE;

	virtual IEffect* Clone()PURE;

	virtual _vec3 Get_Pos() PURE;


	protected:
		virtual void Free();
};

END 

