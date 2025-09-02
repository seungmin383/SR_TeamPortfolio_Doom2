#include "pch.h"
#include "CUIManager.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{
}

CUIManager::~CUIManager()
{
	Free();
}

void CUIManager::InsertUISet(CUISet* uiSet, _bool act, _bool isEnabledFocus)
{
	UISetList.push_back(uiSet);
	Acts.push_back(act);
	FocusEnabled.push_back(isEnabledFocus);
	MaxCount++;

	UISetList[MaxCount - 1]->SetElementsToggle(act);
}

_bool CUIManager::AddUIPhase()
{
	do
	{
		CurUIPhase++;

		if (CurUIPhase > MaxCount - 1)
			CurUIPhase = 0;
	} while (FocusEnabled[CurUIPhase] == false);

	return true;
}

_bool CUIManager::SubTractUIPhase()
{
	do
	{
		CurUIPhase--;

		if (CurUIPhase < 0)
			CurUIPhase = MaxCount - 1;
	} while (FocusEnabled[CurUIPhase] == false);
	
	return true;
}

_bool CUIManager::ToggleUIPhase()
{
	_bool stat = Acts[CurUIPhase];
	UISetList[CurUIPhase]->SetElementsToggle(!stat);
	Acts[CurUIPhase] = !stat;

	return true;
}

_bool CUIManager::ExitUIPhase()
{
	return true;
}

_bool CUIManager::ActivateUIPhase(_int phaseNumber)
{
	return true;
}

void CUIManager::MappingKeyInput(_ubyte dInput, function<_bool()> func)
{
	UIKeyInputMap.insert({ dInput, func });
}

void CUIManager::UpdateUI()
{
	if (UISetList.empty())
		return;

	GetKeyInput();

	UISetList[CurUIPhase]->UpdateUI();
}

void CUIManager::GetKeyInput()
{
	for (auto keymap : UIKeyInputMap)
	{
		if (CDInputManager::GetInstance()->Get_KeyDown(keymap.first))
			if (keymap.second() == false) return;
	}
}

void CUIManager::UISetListClear()
{
	for (auto uiSet : UISetList)
		uiSet->Release();

	UISetList.clear();
	UISetList.shrink_to_fit();

	MaxCount = 0;

	//for (auto keymappair : UIKeyInputMap)
	//	Safe_Delete(keymappair.second);

	UIKeyInputMap.clear();

	Acts.clear();
}

void CUIManager::Free()
{
	UISetListClear();
}
