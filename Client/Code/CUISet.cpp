#include "pch.h"
#include "CUISet.h"

CUISet::CUISet()
{
}

CUISet::~CUISet()
{
	Free();
}

void CUISet::InsertUIObject(CUIObject* uiObj, _bool isEnabledFocus)
{
	UIElements.push_back(uiObj);
	FocusEnabled.push_back(isEnabledFocus);
	MaxCount++;

	uiObj->Add_Ref();
}

_bool CUISet::AddUIElementIndex()
{
	do
	{
		UIElementFocus++;

		if (UIElementFocus > MaxCount - 1)
			UIElementFocus = 0;
	} while (FocusEnabled[UIElementFocus] == false);

	return true;
}

_bool CUISet::SubTractUIElementIndex()
{
	do
	{
		UIElementFocus--;

		if (UIElementFocus < 0)
			UIElementFocus = MaxCount - 1;
	} while (FocusEnabled[UIElementFocus] == false);
	
	return true;
}

_bool CUISet::ActivateUI()
{
	return UIElements[UIElementFocus]->Action();
}

void CUISet::MappingKeyInput(_ubyte dInput, function<_bool()> func)
{
	UIKeyInputMap.insert({ dInput, func });
}

_bool CUISet::ToggleActivate()
{
	return true;
}

void CUISet::UpdateUI()
{
	//레이어에서업데이트해줌
	//키인풋을 포함한 상태 업데이트 관여
	GetKeyInput();
}

void CUISet::SetPosition(_uint num, _float fX, _float fY)
{
	UIElements[num]->Set_Pos(fX, fY);
}

void CUISet::GetKeyInput()
{
	if (UIKeyInputMap.empty())
		return;

	for (auto keymap : UIKeyInputMap)
	{
		if (CDInputManager::GetInstance()->Get_KeyDown(keymap.first))
			if (keymap.second() == false) return;

		if (UIKeyInputMap.empty() || keymap.second == nullptr)
			return;
	}
}

_bool CUISet::SetElementsToggle(_bool bval)
{
	for (auto ele : UIElements)
		ele->Set_IsActive(bval);

	return true;
}

void CUISet::Free()
{
	for (auto uiObj : UIElements)
		uiObj->Release();

	UIElements.clear();
}
