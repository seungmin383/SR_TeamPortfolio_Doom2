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

void CUIManager::InsertUISet(CUISet* uiSet)
{
}

void CUIManager::ToggleUIPhase()
{
}

void CUIManager::ExitUIPhase()
{
}

void CUIManager::ActivateUIPhase(_int phaseNumber)
{
}

void CUIManager::MappingKeyInput(_ubyte dInput, function<void()> func)
{
}

void CUIManager::UpdateUI()
{
}

void CUIManager::GetKeyInput()
{
}

void CUIManager::UISetListClear()
{
	for (auto uiSet : UISetList)
		Safe_Delete(uiSet);

	UISetList.clear();

	for (auto keymappair : UIKeyInputMap)
		Safe_Delete(keymappair.second);
	
	UIKeyInputMap.clear();

	Acts.clear();
}

void CUIManager::Free()
{
	UISetListClear();
}
