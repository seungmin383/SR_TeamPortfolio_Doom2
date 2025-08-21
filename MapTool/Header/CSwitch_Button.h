#pragma once
#include "CGameObject.h"
#include "CDynamicCamera.h"
#include "CPrototypeManager.h"
#include "CDoor.h"

class CSwitch_Button : public CGameObject
{
public:
	enum class LinkMode { OneToOne, OneToMany }; // 스위치별 모드(1:1, 1:N)

private:
	explicit CSwitch_Button(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSwitch_Button(const CGameObject& rhs);
	virtual ~CSwitch_Button();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

	void Set_Target(CDoor* pDoor) 
	{
		m_pObjTarget = pDoor;
		m_target = pDoor;
	}
	void        Clear_Targets()					// 연결 리셋
	{
		m_pObjTarget = nullptr;
		m_target = nullptr;
		m_targets.clear();
	}
	void        Set_LinkMode(LinkMode m) { m_linkMode = m; }
	LinkMode    Get_LinkMode() const { return m_linkMode; }
	// 문이랑 스위치 태그기능
public:
	/// <summary>
	/// 문과 스위치가 1:1일때 사용
	/// </summary>
	/// <param name="id"></param>
	void Set_LinkId(const wstring& id) { m_linkId = id; }
	const wstring& Get_LinkId() const { return m_linkId; }

	/// <summary>
	/// 한 스위치로 여러 문 열거나 이벤트가 나올 때 사용
	/// </summary>
	/// <param name="d"></param>
	void Add_Target(CDoor* d)
	{
		if (!d) return;
		m_targets.push_back(d);
	}

private:
	/// <summary>
	/// 문과 스위치가 1:1일때 사용
	/// </summary>
	wstring m_linkId;
	LinkMode                m_linkMode = LinkMode::OneToOne; // 기본은 1:1

	CDoor* m_target = nullptr;              // 1:1

	/// <summary>
	/// 한 스위치로 여러 문 열거나 이벤트가 나올 때 사용하는 벡터
	/// </summary>
	vector<CDoor*> m_targets;


private:
	bool m_bPressed = false;

private:
	HRESULT Add_Component();
	void        Set_OnTerrain();
	void		Bill_Board();

private:
	Engine::CRectSpriteTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;						// 스위치 초기상태
	Engine::CTexture* m_pTextureCom2;						// 스위치 작동(대부분 켜져 있는걸 끄기)
	Engine::CCalculator* m_pCalculatorCom;

	CDoor* m_pObjTarget;

private:
	enum class State { Idle, Dead };
	State           m_state = State::Idle;

private:
	void SetSwitchOn();

public:
	static CSwitch_Button* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CSwitch_Button* Clone() override;

private:
	virtual void Free();

};

