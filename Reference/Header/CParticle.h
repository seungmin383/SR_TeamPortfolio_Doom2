#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

BEGIN(Engine)
class ENGINE_DLL     CParticle :
    public CGameObject
{
    protected:
    explicit CParticle(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CParticle(const CParticle& rhs);
	virtual ~CParticle();

    public:
    virtual HRESULT         Ready_GameObject() override;
    virtual _int            Update_GameObject(const _float& fTimeDelta) override;
    virtual void            LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void            Render_GameObject(const _float& fTimeDelta) override;

    virtual CGameObject*    Clone() override;
	virtual CGameObject* Clone(CGameObject* _Owner) override;

    virtual HRESULT Add_Component();
public:
	static CParticle* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	CRectSpriteTex* m_pBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;






    private:
		virtual void        Free() override;
};

END
