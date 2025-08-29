#pragma once
#include "IEffect.h"

BEGIN(Engine)

class ENGINE_DLL CEffectBase :public IEffect
{
	protected:
		explicit CEffectBase();
		virtual ~CEffectBase();

};

END